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

# 1. nlohmann/json (рекомендуемая версия 3.11.3, как и было в задании)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
  GIT_SHALLOW TRUE
)

# 2. spdlog (можно взять свежую 1.14.1 или актуальную 1.16.0)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.16.0 #[citation:2] #v1.14.1
  GIT_SHALLOW TRUE
)

# 3. GoogleTest (используйте v1.14.0 для C++14+)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0 #[citation:3][citation:5]
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(json spdlog googletest)

target_link_libraries(calculator math_lib)
