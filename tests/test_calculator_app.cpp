#include <gtest/gtest.h>

#include <iostream>
#include <memory>

#include "CalculatorApp.hpp"
#include "CalculatorExceptions.hpp"

// ==================== ТЕСТОВЫЙ КЛАСС С БД ====================
class CalculatorAppWithDBTest : public ::testing::Test {
   protected:
    std::shared_ptr<Logger> logger;
    std::unique_ptr<CalculatorApp> app;

    void SetUp() override {
        logger = Logger::create();

        // Используем существующую БД calculator_db
        std::string connStr = "host=localhost dbname=calculator_db user=anton password=passWord";

        try {
            app = std::make_unique<CalculatorApp>(logger, connStr);
        } catch(const std::exception& e) {
            GTEST_SKIP() << "Database not available: " << e.what();
        }
    }

    void TearDown() override { app.reset(); }
};

// ==================== ТЕСТЫ ====================

// Тест 1: Базовые операции
TEST_F(CalculatorAppWithDBTest, BasicOperations) {
    EXPECT_EQ(app->calculate(R"({"operand1": 5, "operation": "+", "operand2": 3})"), "8");
    EXPECT_EQ(app->calculate(R"({"operand1": 10, "operation": "-", "operand2": 4})"), "6");
    EXPECT_EQ(app->calculate(R"({"operand1": 3, "operation": "*", "operand2": 7})"), "21");
    EXPECT_EQ(app->calculate(R"({"operand1": 15, "operation": "/", "operand2": 3})"), "5");
    EXPECT_EQ(app->calculate(R"({"operand1": 2, "operation": "^", "operand2": 4})"), "16");
}

// Тест 2: Факториал
TEST_F(CalculatorAppWithDBTest, Factorial) {
    EXPECT_EQ(app->calculate(R"({"operand1": 5, "operation": "!"})"), "120");
    EXPECT_EQ(app->calculate(R"({"operand1": 0, "operation": "!"})"), "1");
    EXPECT_EQ(app->calculate(R"({"operand1": 1, "operation": "!"})"), "1");
}

// Тест 3: Проверка кэширования
TEST_F(CalculatorAppWithDBTest, CacheWorks) {
    // Первый вызов - вычисление
    EXPECT_EQ(app->calculate(R"({"operand1": 7, "operation": "*", "operand2": 8})"), "56");

    // Второй вызов - должен быть из кэша
    EXPECT_EQ(app->calculate(R"({"operand1": 7, "operation": "*", "operand2": 8})"), "56");

    // Выводим статистику для проверки
    std::cout << "\n=== Cache test statistics ===" << std::endl;
    app->printStats();
}

// Тест 4: Проверка работы БД между запусками
TEST_F(CalculatorAppWithDBTest, DatabasePersistence) {
    // Добавляем новую уникальную операцию
    EXPECT_EQ(app->calculate(R"({"operand1": 100, "operation": "+", "operand2": 200})"), "300");

    // Пересоздаем приложение (имитация перезапуска)
    std::string connStr = "host=localhost dbname=calculator_db user=anton password=passWord";
    auto newApp = std::make_unique<CalculatorApp>(logger, connStr);

    // Кэш должен прогреться из БД
    std::cout << "\n=== After restart ===" << std::endl;
    newApp->printStats();
}

// Тест 5: Исключения
TEST_F(CalculatorAppWithDBTest, Exceptions) {
    EXPECT_THROW(app->calculate(R"({"operand1": 5, "operation": "/", "operand2": 0})"), DivisionByZeroException);

    EXPECT_THROW(
        app->calculate(R"({"operand1": 5, "operation": "invalid", "operand2": 3})"), InvalidOperationException
    );

    EXPECT_THROW(app->calculate(R"({"operand1": -5, "operation": "!"})"), NegativeFactorialException);
}

// Тест 6: Невалидный JSON
TEST_F(CalculatorAppWithDBTest, InvalidJson) {
    EXPECT_THROW(app->calculate("not a json"), std::exception);
    EXPECT_THROW(app->calculate(R"({})"), std::exception);
    EXPECT_THROW(app->calculate(R"({"operand1": "abc"})"), std::exception);
}

// Тест 7: Статистика после нескольких операций
TEST_F(CalculatorAppWithDBTest, Statistics) {
    // Очищаем кэш и делаем новые вычисления
    app->calculate(R"({"operand1": 1, "operation": "+", "operand2": 1})");
    app->calculate(R"({"operand1": 1, "operation": "+", "operand2": 1})");  // кэш
    app->calculate(R"({"operand1": 2, "operation": "+", "operand2": 2})");

    std::cout << "\n=== Statistics test ===" << std::endl;
    app->printStats();
}

// Тест 8: Пограничные случаи
TEST_F(CalculatorAppWithDBTest, EdgeCases) {
    EXPECT_EQ(app->calculate(R"({"operand1": -5, "operation": "+", "operand2": 10})"), "5");
    EXPECT_EQ(app->calculate(R"({"operand1": -5, "operation": "*", "operand2": -3})"), "15");
    EXPECT_EQ(app->calculate(R"({"operand1": 0, "operation": "*", "operand2": 100})"), "0");
}

// Тест 9: Проверка, что БД подключена
TEST_F(CalculatorAppWithDBTest, DatabaseConnected) {
    std::cout << "\n=== Database connection test ===" << std::endl;
    app->printStats();  // Должно показать "Database: CONNECTED"
}

// ==================== MAIN ====================
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Using existing database: calculator_db" << std::endl;
    std::cout << "User: anton" << std::endl;

    return RUN_ALL_TESTS();
}