cmake_minimum_required(VERSION 4.0)
project(ITCHFeedhandler)

set(CMAKE_CXX_STANDARD 14)

include_directories(cpp)
add_executable(ITCHFeedhandler
        cpp/include/handlers.h
        cpp/include/itch_messages.h
        cpp/include/itch_parser.h
        cpp/src/main.cpp
        cpp/include/jsonwriter.h
        cpp/include/mapped_file.h
        cpp/include/byte_order.h)

target_include_directories(ITCHFeedhandler PRIVATE cpp/include)