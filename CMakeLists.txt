cmake_minimum_required(VERSION 3.0)
project(WebShellManager)

set(CMAKE_CXX_STANDARD 11)

add_executable(WebShellManager
        cryptohelper.cpp
        cryptohelper.h
        curlhelper.cpp
        curlhelper.h
        json.hpp
        main.cpp
        WebShell.cpp
        WebShell.h
        )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -lcurl -pthread")

target_link_libraries(WebShellManager curl)
