#include "CalculatorExceptions.hpp"

MathException::MathException(const std::string& message) : std::runtime_error("Math error: " + message) {}

DivisionByZeroException::DivisionByZeroException() noexcept : MathException("Division by zero") {}

NegativeFactorialException::NegativeFactorialException() noexcept : MathException("Factorial of negative number") {}

InvalidOperationException::InvalidOperationException(const std::string& oper)
    : MathException("Invalid operation: " + oper) {}

InvalidInputException::InvalidInputException(const std::string& message)
    : std::runtime_error("Input error: " + message) {}