#include <cmath>
#include <cstdlib>  // for std::atof , std::atoi
#include <iostream>
#include <math_utils/math_utils.hpp>
#include <string>

struct Number {
    int first;
    int second;
    std::string operation;
    long long result;
};

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

void parse(int argc, char* argv[], Number& num) {
    // Arguments check
    if(argc < 4) {
        if(argc == 3 && (std::string(argv[2]) == "fact" || std::string(argv[2]) == "!")) {
            // OK call for factorial
        } else {
            print_help();
            return;
        }
    }

    // Parsing
    num.first = std::atoi(argv[1]);
    num.operation = argv[2];
    num.second = (argc >= 4) ? std::atoi(argv[3]) : 0.0;  // for factorial second argument doesn't matter
}

void calc(Number& num) {
    if(num.operation == "add" || num.operation == "+") {
        num.result = math_utils::add(num.first, num.second);
    } else if(num.operation == "sub" || num.operation == "-") {
        num.result = math_utils::subtract(num.first, num.second);
    } else if(num.operation == "mul" || num.operation == "*") {
        num.result = math_utils::multiply(num.first, num.second);
    } else if(num.operation == "div" || num.operation == "/") {
        num.result = math_utils::divide(num.first, num.second);
    } else if(num.operation == "pow" || num.operation == "^") {
        num.result = math_utils::power(num.first, num.second);
    } else if(num.operation == "fact" || num.operation == "!") {
        num.result = math_utils::factorial(num.first);
    } else {
        if(num.operation != "0")  // prevention of initial two-time print_help()
        {
            std::cout << "Error: unknown operation " << num.operation << ".\n";
            print_help();
            return;
        }
    }
}

void printResult(Number& num) { std::cout << "Result = " << num.result << std::endl; }

void run(int argc, char* argv[]) {
    Number num = {0, 0, "0", 0};
    parse(argc, argv, num);
    calc(num);
    printResult(num);
}
