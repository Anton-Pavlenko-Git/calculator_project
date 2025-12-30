#pragma once

#include <stdexcept>
#include <string>

class MathException : public std::runtime_error {
   public:
    explicit MathException(const std::string& message);
};

class DivisionByZeroException : public MathException {
   public:
    DivisionByZeroException() noexcept;
};

class NegativeFactorialException : public MathException {
   public:
    NegativeFactorialException() noexcept;
};

class InvalidOperationException : public MathException {
   public:
    explicit InvalidOperationException(const std::string& oper);
};

class InvalidInputException : public std::runtime_error {
   public:
    explicit InvalidInputException(const std::string& message);
};