cmake_minimum_required(VERSION 3.10)

project(Calculator)

set(CMAKE_CXX_STANDART 11)

add_executable(calculator src/main.cpp math_lib/math_lib.cpp)
