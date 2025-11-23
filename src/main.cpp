#include <iostream>
#include <string>
#include <cstdlib> // for std::atof , std::atoi
#include <cmath>
#include "../math_lib/math_lib.h"

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

int main (int argc, char* argv[]) {
	if(argc<4) {
		if(argc == 3 && (std::string(argv[2]) == "fact" || std::string(argv[2]) == "!")) {
		// OK call for factorial
		} else {
			print_help();
			return 1;
		}
	}

	// Parsing arguments
	double num1 = std::atof(argv[1]);
	std::string operation = argv[2];
	double num2 = (argc >= 4) ? std::atof(argv[3]) : 0.0; // for factorial second argument doesn't matter

	double result_double = 0.0;
	long long result_long_long = 0;
	bool is_double_result = true;

	// Executing operation
	if(operation == "add" || operation == "+") {
		result_double = add(num1,num2);
	} else if (operation == "sub" || operation == "-") {
		result_double = subtract(num1,num2);
	} else if (operation == "mul" || operation == "*") {
		result_double = multiply(num1,num2);
	} else if (operation == "div" || operation == "/") {
		result_double = divide(num1,num2);
		if (std::isnan(result_double)) {
			std::cout << "Error: dision by zero!" << std::endl;
			return 1;
		}
	} else if (operation == "pow" || operation == "^")  {
		// for power second argument must be int
		int exponent = static_cast<int>(num2);
		result_double = power(num1, exponent);
	} else if (operation == "fact" || operation == "!") {
		int n = static_cast<int>(num1);
		result_long_long = factorial(n);
		is_double_result = false;
		if (result_long_long == -1) {
			std::cout << "Error: factorial olny positive integers!" << std::endl;
			return 1;
		}
	} else {
		std::cout << "Error: unknown operation " << operation << ".\n";
		print_help();
		return 1;
	}

	// Result out
	if(is_double_result) {
		std::cout << "Result: " << result_double << std::endl;
	} else {
		std::cout << "Result: " << result_long_long << std::endl;
	}

	return 0;
}
