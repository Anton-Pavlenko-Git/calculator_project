#include <gtest/gtest.h>

#include <iostream>
#include <memory>

#include "CalculatorApp.hpp"
#include "CalculatorExceptions.hpp"
/*
// Временный логгер, который пишет в stdout (для тестов)
class CoutLogger : public Logger {
   public:
    void info(const std::string& message) override { std::cout << "[INFO] " << message << '\n'; }

    void error(const std::string& message) override { std::cout << "[ERROR] " << message << '\n'; }

    void debug(const std::string& message) override { std::cout << "[DEBUG] " << message << '\n'; }
};
*/
// Тест 1: Парсинг JSON
TEST(CalculatorAppTest, BasicOperations) {
    auto logger = Logger::create();
    CalculatorApp app(logger);

    EXPECT_EQ(app.calculate(R"({"operand1": 5, "operation": "+", "operand2": 3})"), "8");
    EXPECT_EQ(app.calculate(R"({"operand1": 5, "operation": "!"})"), "120");
}

// Тест 2: Парсинг факториала (без operand2)
TEST(CalculatorAppTest, ParseFactorialJson) {
    auto logger = Logger::create();
    CalculatorApp app(logger);

    EXPECT_EQ(app.calculate(R"({"operand1": 5, "operation": "!"})"), "120");
}

// Тест 3: Исключения
TEST(CalculatorAppTest, DivisionByZeroThrows) {
    auto logger = Logger::create();
    CalculatorApp app(logger);

    EXPECT_THROW(app.calculate(R"({"operand1": 5, "operation": "/", "operand2": 0})"), DivisionByZeroException);
}

// Тест 4: Неверная операция
TEST(CalculatorAppTest, InvalidOperationThrows) {
    auto logger = Logger::create();
    CalculatorApp app(logger);

    EXPECT_THROW(
        app.calculate(R"({"operand1": 5, "operation": "invalid_op", "operand2": 3})"), InvalidOperationException
    );
}

// Тест 5: Невалидный JSON
TEST(CalculatorAppTest, InvalidJsonThrows) {
    auto logger = Logger::create();
    CalculatorApp app(logger);

    EXPECT_THROW(app.calculate("not a json"), InvalidInputException);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}