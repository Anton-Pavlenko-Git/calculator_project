cmake_minimum_required(VERSION 3.20)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wpedantic -Werror)

include(FetchContent)

FetchContent_Declare(
  math_lib
  GIT_REPOSITORY https://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@github.com/Anton-Pavlenko-Git/math-library
  GIT_TAG main
)

FetchContent_MakeAvailable(math_lib)

add_executable(calculator
    src/main.cpp
    src/calc.cpp
)

target_link_libraries(calculator math_lib)
