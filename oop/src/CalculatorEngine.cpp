#include "CalculatorEngine.hpp"

#include <math_utils/math_utils.hpp>

#include "CalculatorExceptions.hpp"

int CalculatorEngine::calculate(const CalculationRequest& request) const {
    const std::string& op = request.getOperation();
    if(op == "+" || op == "add") {
        return math_utils::add(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    } else if(op == "-" || op == "sub") {
        return math_utils::subtract(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    } else if(op == "*" || op == "mul") {
        return math_utils::multiply(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    } else if(op == "/" || op == "div") {
        int second = request.getSecondOperand().value_or(0);
        if(second == 0) {
            throw DivisionByZeroException();
        }
        return math_utils::divide(request.getFirstOperand(), second);
    } else if(op == "^" || op == "pow") {
        return math_utils::power(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    } else if(op == "!" || op == "fact") {
        int n = request.getFirstOperand();
        if(n < 0) {
            throw NegativeFactorialException();
        }
        return math_utils::factorial(n);
    }

    throw InvalidOperationException(op);
}