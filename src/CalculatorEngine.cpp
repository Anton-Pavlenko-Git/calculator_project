#include "CalculatorEngine.hpp"

#include <math_utils/math_utils.hpp>

#include "CalculatorExceptions.hpp"

int CalculatorEngine::calculate(const CalculationRequest& request) const {
    const std::string& oper = request.getOperation();
    if(oper == "+" || oper == "add") {
        return math_utils::add(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    }
    if(oper == "-" || oper == "sub") {
        return math_utils::subtract(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    }
    if(oper == "*" || oper == "mul") {
        return math_utils::multiply(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    }
    if(oper == "/" || oper == "div") {
        const int second = request.getSecondOperand().value_or(0);
        if(second == 0) {
            throw DivisionByZeroException();
        }
        return math_utils::divide(request.getFirstOperand(), second);
    }
    if(oper == "^" || oper == "pow") {
        return math_utils::power(request.getFirstOperand(), request.getSecondOperand().value_or(0));
    }
    if(oper == "!" || oper == "fact") {
        const int num = request.getFirstOperand();
        if(num < 0) {
            throw NegativeFactorialException();
        }
        return math_utils::factorial(num);
    }

    throw InvalidOperationException(oper);
}
