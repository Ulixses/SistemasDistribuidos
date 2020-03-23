cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Debug)
project(http_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
add_executable(http_server main.cpp httpserver.cpp httpserver.h utils.cpp utils.h)
