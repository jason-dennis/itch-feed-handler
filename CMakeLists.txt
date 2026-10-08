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

target_include_directories(ITCHFeedhandler PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
        ${GEN_DIR})

# --- warning-uri ---
if(MSVC)
    target_compile_options(ITCHFeedhandler PRIVATE /W4 /permissive-)
else()
    target_compile_options(ITCHFeedhandler PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- optimizare pentru procesorul curent ---
# (Release aduce deja -O3 pe GCC/Clang si /O2 pe MSVC)
if(ITCH_NATIVE AND NOT MSVC)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-march=native" ITCH_HAS_MARCH_NATIVE)
    if(ITCH_HAS_MARCH_NATIVE)
        target_compile_options(ITCHFeedhandler PRIVATE -march=native)
    else()
        message(STATUS "-march=native not supported by this compiler, skipping")
    endif()
endif()

# --- sanitizers ---
if(ITCH_SANITIZE)
    if(MSVC)
        target_compile_options(ITCHFeedhandler PRIVATE /fsanitize=address)
    else()
        set(ITCH_SAN_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
        target_compile_options(ITCHFeedhandler PRIVATE ${ITCH_SAN_FLAGS})
        target_link_options(ITCHFeedhandler PRIVATE ${ITCH_SAN_FLAGS})
    endif()
endif()

# --- profile-guided optimization ---
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