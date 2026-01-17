#include "CalculatorApp.hpp"

#include <iostream>

#include "CalculationRequest.hpp"
#include "CalculatorExceptions.hpp"

// Конструктор без БД (для обратной совместимости)
CalculatorApp::CalculatorApp(std::shared_ptr<Logger> logger)
    : logger_(std::move(logger)), engine_(std::make_unique<CalculatorEngine>()) {
    if(!logger_) {
        throw std::invalid_argument("Logger cannot be null");
    }
}

// Конструктор с БД
CalculatorApp::CalculatorApp(std::shared_ptr<Logger> logger, const std::string& databaseConnectionString)
    : logger_(std::move(logger)), engine_(std::make_unique<CalculatorEngine>(databaseConnectionString)) {
    if(!logger_) {
        throw std::invalid_argument("Logger cannot be null");
    }

    logger_->info("Calculator application started with database support");
}

void CalculatorApp::run() {
    logger_->info("Calculator application started");

    std::cout << "=== JSON Calculator ===" << '\n';
    printHelp();

    if(engine_->hasDatabase()) {
        std::cout << "Database: CONNECTED (caching enabled)" << '\n';
    } else {
        std::cout << "Database: NOT CONNECTED (no caching)" << '\n';
    }

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

        if(line == "stats") {
            printStats();
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

std::string CalculatorApp::calculate(const std::string& jsonInput) {
    // 1. Парсим JSON в CalculationRequest
    CalculationRequest const request = CalculationRequest::fromJson(jsonInput);
    // 2. Выполняем вычисление
    int const result = engine_->calculate(request);
    // 3. Возвращаем результат в processInput как строку
    return std::to_string(result);
}

void CalculatorApp::processInput(const std::string& jsonInput) {
    std::string result = calculate(jsonInput);
    std::cout << "Result: " << result << '\n';

    logger_->info("Calculation successful: " + jsonInput + " = " + result);
}

void CalculatorApp::printHelp() noexcept {
    std::cout << "\nFormat: {\"operand1\": 5, \"operation\": \"+\", \"operand2\": 3}" << '\n';
    std::cout << "Operations: +  -  *  /  ^  !" << '\n';
    std::cout << R"(For factorial: {"operand1": 5, "operation": "!"})" << '\n';
    std::cout << "Commands: help, exit" << '\n';
}

void CalculatorApp::printStats() const {
    auto stats = engine_->getStats();

    std::cout << "\n=== Calculator Statistics ===" << '\n';
    std::cout << "Total calculations: " << stats.calculations << '\n';
    std::cout << "Cache hits: " << stats.cacheHits << " (" << stats.cacheHitRate() << "%)" << '\n';
    std::cout << "DB hits: " << stats.dbHits << " (" << stats.dbHitRate() << "%)" << '\n';
    std::cout << "Cache size: " << stats.cacheSize << " items" << '\n';
    std::cout << "Database: " << (engine_->hasDatabase() ? "CONNECTED" : "NOT CONNECTED") << '\n';
}