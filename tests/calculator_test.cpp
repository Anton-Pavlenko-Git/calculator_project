#include <gtest/gtest.h>

#include "CalculationRequest.hpp"
#include "CalculatorEngine.hpp"
#include "CalculatorExceptions.hpp"

// Тест 1: Парсинг JSON
TEST(CalculationRequestTest, ParseValidJson) {
    std::string json = R"({"operand1": 5, "operation": "+", "operand2": 3})";
    CalculationRequest req = CalculationRequest::fromJson(json);

    EXPECT_EQ(req.getFirstOperand(), 5);
    EXPECT_EQ(req.getOperation(), "+");
    EXPECT_TRUE(req.getSecondOperand().has_value());
    EXPECT_EQ(req.getSecondOperand().value(), 3);
}

// Тест 2: Парсинг факториала (без operand2)
TEST(CalculationRequestTest, ParseFactorialJson) {
    std::string json = R"({"operand1": 5, "operation": "!"})";
    CalculationRequest req = CalculationRequest::fromJson(json);

    EXPECT_EQ(req.getFirstOperand(), 5);
    EXPECT_EQ(req.getOperation(), "!");
    EXPECT_FALSE(req.getSecondOperand().has_value());  // Должно быть nullopt
}

// Тест 3: Вычисление операций
TEST(CalculatorEngineTest, BasicOperations) {
    CalculatorEngine engine;

    CalculationRequest add(5, "+", 3);
    EXPECT_EQ(engine.calculate(add), 8);

    CalculationRequest mul(5, "*", 3);
    EXPECT_EQ(engine.calculate(mul), 15);

    CalculationRequest fact(5, "!");
    EXPECT_EQ(engine.calculate(fact), 120);
}

// Тест 4: Исключения
TEST(CalculatorEngineTest, DivisionByZeroThrows) {
    CalculatorEngine engine;
    CalculationRequest div(5, "/", 0);

    EXPECT_THROW(engine.calculate(div), DivisionByZeroException);
}

// Тест 5: Неверная операция
TEST(CalculatorEngineTest, InvalidOperationThrows) {
    CalculatorEngine engine;
    CalculationRequest invalid(5, "invalid_op", 3);

    EXPECT_THROW(engine.calculate(invalid), InvalidOperationException);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}