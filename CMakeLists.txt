cmake_minimum_required(VERSION 3.20)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(math-library)

add_executable(calculator 
    src/main.cpp
    # src/calc.cpp
)

target_link_libraries(calculator math_lib)
