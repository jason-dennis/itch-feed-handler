cmake_minimum_required(VERSION 3.16)
project(ITCHFeedhandler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# --- cod generat din messagesSpecs.json ---
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

# --- executabil ---
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