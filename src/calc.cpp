#include <cmath>
#include <cstdlib>  // for std::atoi
#include <iostream>
#include <math_utils/math_utils.hpp>
#include <string>

#include "header.hpp"

void print_help() {
    std::cout << "Usage: ./calculator <num1> <operation> <num2>\n";
    std::cout << "Avaliable operations:\n";
    std::cout << " add	(+)\n";
    std::cout << " sub	(-)\n";
    std::cout << " mul	(*)\n";
    std::cout << " div	(/)\n";
    std::cout << " pow 	(^)\n";
    std::cout << " fact	(!)\n\n";
    std::cout << "Example:\n";
    std::cout << " ./calculator 5 add 3\n";
    std::cout << " ./calculator 10 div 2\n";
    std::cout << " ./calculator 2 pow 8\n";
    std::cout << " ./calculator 5 fact 0\n";
}

int argsCheck(int argc, char* argv[]) {
    // Быстрая проверка на недостаточное количество аргументов
    if(argc < 3) {
        return -1;
    }

    // Проверка валидности комбинации аргументов
    bool isFactorial = (argv[2] == "fact" || argv[2] == "!");
    bool isValid = (isFactorial && argc == 3) || (!isFactorial && argc >= 4);
    if(!isValid) {
        return -2;
    }

    return 0;
}

int parse(int argc, char* argv[], Number& num) {
    num.first = std::atoi(argv[1]);
    num.operation = argv[2];
    num.second = (num.operation == "fact" || num.operation == "!") ? 0 : std::atoi(argv[3]);

    return 0;
}

int calc(Number& num) {
    if(num.operation == "add" || num.operation == "+") {
        num.result = math_utils::add(num.first, num.second);
        return 0;
    } else if(num.operation == "sub" || num.operation == "-") {
        num.result = math_utils::subtract(num.first, num.second);
        return 0;
    } else if(num.operation == "mul" || num.operation == "*") {
        num.result = math_utils::multiply(num.first, num.second);
        return 0;
    } else if(num.operation == "div" || num.operation == "/") {
        num.result = math_utils::divide(num.first, num.second);
        return 0;
    } else if(num.operation == "pow" || num.operation == "^") {
        num.result = math_utils::power(num.first, num.second);
        return 0;
    } else if(num.operation == "fact" || num.operation == "!") {
        num.result = math_utils::factorial(num.first);
        return 0;
    } else {
        return 1;
    }
}

void printResult(Number& num) { std::cout << "Result = " << num.result << std::endl; }

void runCalculator(int argc, char* argv[]) {
    Number num;
    if(!argsCheck(argc, argv)) {
        parse(argc, argv, num);

        if(!calc(num)) {
            printResult(num);
        } else {
            std::cout << "Error: unknown operation " << num.operation << ".\n";
            print_help();
        }
    } else {
        std::cout << "Invalid number of arguments.\n";
        print_help();
    }
}
