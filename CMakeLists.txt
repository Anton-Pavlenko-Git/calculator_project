cmake_minimum_required(VERSION 3.20)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

add_executable(calculator
    src/main.cpp
    src/calc.cpp
)

set_target_properties(calculator PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_compile_options(-Wall -Wpedantic -Werror)

include(FetchContent)

FetchContent_Declare(
  math_lib
  GIT_REPOSITORY https://github.com/Anton-Pavlenko-Git/math-library
  GIT_TAG main
)

FetchContent_MakeAvailable(math_lib)

target_link_libraries(calculator math_lib)
