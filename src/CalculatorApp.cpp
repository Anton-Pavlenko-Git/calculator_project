#include "CalculatorApp.hpp"

#include <iostream>

#include "CalculationRequest.hpp"
#include "CalculatorExceptions.hpp"

CalculatorApp::CalculatorApp(std::shared_ptr<Logger> logger) : logger_(std::move(logger)) {
    if(!logger_) {
        throw std::invalid_argument("Logger cannot be null");
    }
}

void CalculatorApp::run() {
    logger_->info("Calculator application started");

    std::cout << "=== JSON Calculator ===" << '\n';
    printHelp();

    std::string line;
    while(true) {
        std::cout << "\nEnter JSON or 'help'/'exit': ";

        if(!std::getline(std::cin, line)) {
            break;  // Ctrl+D
        }
        if(line.empty()) {
            continue;
        }

        if(line == "exit" || line == "quit") {
            break;
        }

        if(line == "help") {
            printHelp();
            continue;
        }

        try {
            processInput(line);
        } catch(const std::exception& e) {
            logger_->error("Processing error: " + std::string(e.what()));
            std::cerr << "Error: " << e.what() << '\n';
        }
    }

    logger_->info("Calculator application finished");
    std::cout << "Goodbye!" << '\n';
}

void CalculatorApp::processInput(const std::string& jsonInput) {
    // 1. Парсим JSON в CalculationRequest
    CalculationRequest const request = CalculationRequest::fromJson(jsonInput);

    // 2. Выполняем вычисление
    long long const result = engine_.calculate(request);

    // 3. Выводим результат
    std::cout << "Result: " << result << '\n';

    // 4. Логируем успех
    logger_->info("Calculation successful: " + request.toJson() + " = " + std::to_string(result));
}

void CalculatorApp::printHelp() {
    std::cout << "\nFormat: {\"operand1\": 5, \"operation\": \"+\", \"operand2\": 3}" << '\n';
    std::cout << "Operations: +  -  *  /  ^  !" << '\n';
    std::cout << R"(For factorial: {"operand1": 5, "operation": "!"})" << '\n';
    std::cout << "Commands: help, exit" << '\n';
}