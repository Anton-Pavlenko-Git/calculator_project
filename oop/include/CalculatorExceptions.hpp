#pragma once

#include <stdexcept>
#include <string>

class MathException : std::runtime_error {
   public:
    explicit MathException(const std::string& message) : std::runtime_error("Math error: " + message) {}
};

class DivisionByZeroException : public MathException {
   public:
    DivisionByZeroException() : MathException("Division by zero") {}
};

class NegativeFactorialException : public MathException {
   public:
    NegativeFactorialException() : MathException("Factorial of negative number") {};
};

class InvalidOperationException : public MathException {
   public:
    InvalidOperationException(const std::string& op) : MathException("Invalid operation" + op) {}
};

class InvalidInputException : public std::runtime_error {
   public:
    explicit InvalidInputException(const std::string& message) : std::runtime_error("Input error: " + message) {}
};