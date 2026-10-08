cmake_minimum_required(VERSION 3.16)
project(ITCHFeedhandler LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Setari generale
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fara tip de build ales -> Release (altfel build-ul ar fi neoptimizat).
# Generatoarele multi-config (Visual Studio, Xcode) aleg tipul la build.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

option(ITCH_NATIVE   "Optimize for the CPU of this machine (-march=native)" ON)
option(ITCH_SANITIZE "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
set(ITCH_PGO "OFF" CACHE STRING "Profile-guided optimization: OFF, GENERATE or USE")
set_property(CACHE ITCH_PGO PROPERTY STRINGS OFF GENERATE USE)
set(ITCH_PGO_DIR "${CMAKE_BINARY_DIR}/pgo" CACHE PATH "Where PGO profiles are written/read")

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# ---------------------------------------------------------------------------
# Cod generat din messagesSpecs.json
# ---------------------------------------------------------------------------
set(GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpp/generated)
set(GEN_HEADERS
        ${GEN_DIR}/messages_gen.h
        ${GEN_DIR}/json_gen.h)

# regenereaza la build, cand se schimba specificatiile sau generatorul
add_custom_command(
        OUTPUT ${GEN_HEADERS}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GEN_DIR}
        COMMAND Python3::Interpreter
        ${CMAKE_CURRENT_SOURCE_DIR}/python/generate.py
        ${CMAKE_CURRENT_SOURCE_DIR}/data/messagesSpecs.json
        ${GEN_HEADERS}
        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/python/generate.py
        ${CMAKE_CURRENT_SOURCE_DIR}/data/messagesSpecs.json
        COMMENT "Generating ITCH message code from messagesSpecs.json"
        VERBATIM)

# genereaza si la configurare, ca IDE-ul sa gaseasca headerele inainte de primul build
if(NOT EXISTS ${GEN_DIR}/messages_gen.h OR NOT EXISTS ${GEN_DIR}/json_gen.h)
    file(MAKE_DIRECTORY ${GEN_DIR})
    execute_process(
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/python/generate.py
            ${CMAKE_CURRENT_SOURCE_DIR}/data/messagesSpecs.json
            ${GEN_HEADERS}
            RESULT_VARIABLE GEN_RESULT)
    if(NOT GEN_RESULT EQUAL 0)
        message(FATAL_ERROR "generate.py failed")
    endif()
endif()

# ---------------------------------------------------------------------------
# Biblioteca "itch": headerele + codul generat, folosite de executabil si de teste
# ---------------------------------------------------------------------------
add_custom_target(itch_generate DEPENDS ${GEN_HEADERS})

add_library(itch INTERFACE)
target_include_directories(itch INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
        ${GEN_DIR})
target_compile_features(itch INTERFACE cxx_std_17)

# Setarile comune pentru orice target care foloseste biblioteca:
# generare, warning-uri, -march=native, sanitizers.
function(itch_setup_target target)
    add_dependencies(${target} itch_generate)
    target_link_libraries(${target} PRIVATE itch)

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(ITCH_NATIVE AND NOT MSVC)
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-march=native" ITCH_HAS_MARCH_NATIVE)
        if(ITCH_HAS_MARCH_NATIVE)
            target_compile_options(${target} PRIVATE -march=native)
        endif()
    endif()

    if(ITCH_SANITIZE)
        if(MSVC)
            target_compile_options(${target} PRIVATE /fsanitize=address)
        else()
            set(san -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
            target_compile_options(${target} PRIVATE ${san})
            target_link_options(${target} PRIVATE ${san})
        endif()
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Executabil
# ---------------------------------------------------------------------------
add_executable(ITCHFeedhandler
        cpp/src/main.cpp
        cpp/include/byte_order.h
        cpp/include/handlers.h
        cpp/include/itch_parser.h
        cpp/include/jsonwriter.h
        cpp/include/mapped_file.h
        ${GEN_HEADERS})
itch_setup_target(ITCHFeedhandler)

# --- profile-guided optimization (doar pentru executabil) ---
# Pasii: configurare cu GENERATE + build, o rulare pe date reale, apoi
# reconfigurare cu USE + build, in ACELASI folder de build.
if(NOT ITCH_PGO STREQUAL "OFF")
    file(MAKE_DIRECTORY ${ITCH_PGO_DIR})
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(ITCH_PGO STREQUAL "GENERATE")
            set(ITCH_PGO_FLAGS -fprofile-generate=${ITCH_PGO_DIR})
        elseif(ITCH_PGO STREQUAL "USE")
            set(ITCH_PGO_FLAGS -fprofile-use=${ITCH_PGO_DIR} -fprofile-correction -Wno-missing-profile)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(ITCH_PGO STREQUAL "GENERATE")
            set(ITCH_PGO_FLAGS -fprofile-instr-generate=${ITCH_PGO_DIR}/itch-%p.profraw)
        elseif(ITCH_PGO STREQUAL "USE")
            # Clang cere un pas de "merge" al profilelor brute intr-un singur fisier
            find_program(LLVM_PROFDATA NAMES llvm-profdata llvm-profdata-20 llvm-profdata-19 llvm-profdata-18 llvm-profdata-17)
            if(NOT LLVM_PROFDATA AND APPLE)
                execute_process(COMMAND xcrun --find llvm-profdata
                        OUTPUT_VARIABLE LLVM_PROFDATA OUTPUT_STRIP_TRAILING_WHITESPACE)
            endif()
            file(GLOB ITCH_PROFRAW ${ITCH_PGO_DIR}/*.profraw)
            if(NOT LLVM_PROFDATA OR NOT ITCH_PROFRAW)
                message(FATAL_ERROR "ITCH_PGO=USE: llvm-profdata or *.profraw files in ${ITCH_PGO_DIR} not found")
            endif()
            execute_process(COMMAND ${LLVM_PROFDATA} merge -output=${ITCH_PGO_DIR}/merged.profdata ${ITCH_PROFRAW}
                    RESULT_VARIABLE MERGE_RESULT)
            if(NOT MERGE_RESULT EQUAL 0)
                message(FATAL_ERROR "llvm-profdata merge failed")
            endif()
            set(ITCH_PGO_FLAGS -fprofile-instr-use=${ITCH_PGO_DIR}/merged.profdata)
        endif()
    else()
        message(WARNING "ITCH_PGO is only supported with GCC and Clang, ignoring")
    endif()

    if(ITCH_PGO_FLAGS)
        target_compile_options(ITCHFeedhandler PRIVATE ${ITCH_PGO_FLAGS})
        target_link_options(ITCHFeedhandler PRIVATE ${ITCH_PGO_FLAGS})
        message(STATUS "PGO: ${ITCH_PGO}")
    endif()
endif()

# ---------------------------------------------------------------------------
# Teste (GoogleTest)
# ---------------------------------------------------------------------------
option(ITCH_BUILD_TESTS "Build the unit tests" ON)

if(ITCH_BUILD_TESTS)
    include(FetchContent)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_Declare(googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # necesar pe MSVC
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(itch_tests tests/test_parser.cpp)
    itch_setup_target(itch_tests)
    target_link_libraries(itch_tests PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(itch_tests)

    # C++ contra oracolul Python, pe fixture-urile din tests/data
    add_test(NAME oracle_comparison
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_with_oracle.py
            $<TARGET_FILE:ITCHFeedhandler>
            ${CMAKE_CURRENT_SOURCE_DIR}/data/messagesSpecs.json
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/data
            ${CMAKE_CURRENT_SOURCE_DIR}/python
            ${CMAKE_CURRENT_BINARY_DIR}/oracle_comparison)
endif()