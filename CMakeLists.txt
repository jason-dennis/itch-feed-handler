cmake_minimum_required(VERSION 4.0)
project(ITCHFeedhandler)

set(CMAKE_CXX_STANDARD 14)

include_directories(cpp)

add_executable(ITCHFeedhandler
        cpp/handlers.h
        cpp/itch_messages.h
        cpp/itch_parser.h
        cpp/main.cpp
        cpp/jsonwriter.h)
