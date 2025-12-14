cmake_minimum_required(VERSION 3.20)

# Исполняемый файл тестов
add_executable(calculator_tests
    calculator_test.cpp
    ${CMAKE_SOURCE_DIR}/src/CalculationRequest.cpp
    ${CMAKE_SOURCE_DIR}/src/CalculatorEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/InputParser.cpp
    ${CMAKE_SOURCE_DIR}/src/Logger.cpp
)

# Подключаем заголовки
target_include_directories(calculator_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${math_lib_SOURCE_DIR}/include
)

# Линкуем библиотеки
target_link_libraries(calculator_tests PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    GTest::gtest_main
)

# Настройка C++20
set_target_properties(calculator_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Добавляем в CTest
add_test(NAME CalculatorTests COMMAND calculator_tests)