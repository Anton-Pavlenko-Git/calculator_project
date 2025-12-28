cmake_minimum_required(VERSION 3.20)
project(Calculator VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

add_executable(calculator
    src/main.cpp
    src/CalculationRequest.cpp
    src/CalculatorEngine.cpp
    src/Logger.cpp
    src/CalculatorApp.cpp
    src/CalculatorExceptions.cpp
)

set_target_properties(calculator PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_compile_options(-Wall -Wpedantic -Werror)

# 0. My lib
include(FetchContent)

FetchContent_Declare(
  math_lib
  GIT_REPOSITORY https://github.com/Anton-Pavlenko-Git/math-library
  GIT_TAG main
  GIT_SHALLOW TRUE
)
#FetchContent_MakeAvailable(math_lib)

# 1. nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
  GIT_SHALLOW TRUE
)

# 2. spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.16.0
  GIT_SHALLOW TRUE
)

# 3. GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(math_lib nlohmann_json spdlog googletest)

target_include_directories(calculator PRIVATE
  include
  #${math_lib_SOURCE_DIR}/include
)

target_link_libraries(calculator PRIVATE
  nlohmann_json::nlohmann_json
  spdlog::spdlog
  math_utils
)

# Включаем тестирование
enable_testing()

# Подключаем тесты если GoogleTest доступен
if(TARGET GTest::gtest_main)
    add_subdirectory(tests)
endif()

# clang-tidy
#if(USE_CLANG_TIDY)
#    find_program(CLANG_TIDY_PATH NAMES clang-tidy-21 REQUIRED)
#    message(STATUS "Found clang-tidy: ${CLANG_TIDY_PATH}")
#    set(CLANG_TIDY_COMMAND ${CLANG_TIDY_PATH} "-config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
#    set_target_properties(${Calculator} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
#endif()

# clang-tidy
option(USE_CLANG_TIDY "Enable clang-tidy checks" ON)

if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE 
        NAMES clang-tidy-21 clang-tidy
        DOC "Path to clang-tidy executable"
    )
    
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        
        # Правильная настройка clang-tidy
        set(CMAKE_CXX_CLANG_TIDY 
            ${CLANG_TIDY_EXE}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            -extra-arg=-std=c++20
        )
        
        # Также можно добавить отдельную цель для проверки
        add_custom_target(run-clang-tidy
            COMMAND ${CLANG_TIDY_EXE}
                    --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
                    -p ${CMAKE_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src/*.cpp
                    ${CMAKE_SOURCE_DIR}/include/*.h
            COMMENT "Running clang-tidy on all source files"
            VERBATIM
        )
        
    else()
        message(WARNING "clang-tidy requested but executable not found")
    endif()
endif()