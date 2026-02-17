#include <grpcpp/grpcpp.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "Logger.hpp"
#include "calculator.grpc.pb.h"

class CalculatorClient {
   public:
    CalculatorClient(std::shared_ptr<grpc::Channel> channel, std::shared_ptr<Logger> logger)
        : stub_(calculator::Calculator::NewStub(channel)), logger_(std::move(logger)) {}

    // Тест сложения
    bool testAddition(int a, int b, int expected) {
        calculator::CalculationRequest request;
        request.set_operand1(a);
        request.set_operation("+");
        request.set_operand2(b);

        calculator::CalculationResponse response;
        grpc::ClientContext context;

        auto status = stub_->Calculate(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        bool passed = (response.result() == expected);
        std::string result = "Test " + std::to_string(a) + " + " + std::to_string(b) + " = " +
                             std::to_string(response.result()) + " (expected " + std::to_string(expected) +
                             ") : " + (passed ? "PASSED" : "FAILED");

        if(passed) {
            logger_->info(result);
        } else {
            logger_->error(result);
        }

        return passed;
    }

    // Тест вычитания
    bool testSubtraction(int a, int b, int expected) {
        calculator::CalculationRequest request;
        request.set_operand1(a);
        request.set_operation("-");
        request.set_operand2(b);

        calculator::CalculationResponse response;
        grpc::ClientContext context;

        auto status = stub_->Calculate(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        bool passed = (response.result() == expected);
        std::string result = "Test " + std::to_string(a) + " - " + std::to_string(b) + " = " +
                             std::to_string(response.result()) + " (expected " + std::to_string(expected) +
                             ") : " + (passed ? "PASSED" : "FAILED");

        if(passed) {
            logger_->info(result);
        } else {
            logger_->error(result);
        }

        return passed;
    }

    // Тест умножения
    bool testMultiplication(int a, int b, int expected) {
        calculator::CalculationRequest request;
        request.set_operand1(a);
        request.set_operation("*");
        request.set_operand2(b);

        calculator::CalculationResponse response;
        grpc::ClientContext context;

        auto status = stub_->Calculate(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        bool passed = (response.result() == expected);
        std::string result = "Test " + std::to_string(a) + " * " + std::to_string(b) + " = " +
                             std::to_string(response.result()) + " (expected " + std::to_string(expected) +
                             ") : " + (passed ? "PASSED" : "FAILED");

        if(passed) {
            logger_->info(result);
        } else {
            logger_->error(result);
        }

        return passed;
    }

    // Тест деления
    bool testDivision(int a, int b, int expected) {
        calculator::CalculationRequest request;
        request.set_operand1(a);
        request.set_operation("/");
        request.set_operand2(b);

        calculator::CalculationResponse response;
        grpc::ClientContext context;

        auto status = stub_->Calculate(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        bool passed = (response.result() == expected);
        std::string result = "Test " + std::to_string(a) + " / " + std::to_string(b) + " = " +
                             std::to_string(response.result()) + " (expected " + std::to_string(expected) +
                             ") : " + (passed ? "PASSED" : "FAILED");

        if(passed) {
            logger_->info(result);
        } else {
            logger_->error(result);
        }

        return passed;
    }

    // Тест факториала
    bool testFactorial(int a, int expected) {
        calculator::CalculationRequest request;
        request.set_operand1(a);
        request.set_operation("!");
        request.set_operand2(0);

        calculator::CalculationResponse response;
        grpc::ClientContext context;

        auto status = stub_->Calculate(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        bool passed = (response.result() == expected);
        std::string result = "Test " + std::to_string(a) + "! = " + std::to_string(response.result()) + " (expected " +
                             std::to_string(expected) + ") : " + (passed ? "PASSED" : "FAILED");

        if(passed) {
            logger_->info(result);
        } else {
            logger_->error(result);
        }

        return passed;
    }

    // Тест деления на ноль (ожидаем ошибку)
    bool testDivisionByZero(int a) {
        calculator::CalculationRequest request;
        request.set_operand1(a);
        request.set_operation("/");
        request.set_operand2(0);

        calculator::CalculationResponse response;
        grpc::ClientContext context;

        auto status = stub_->Calculate(&context, request, &response);

        bool passed = !status.ok();
        std::string result = "Test " + std::to_string(a) + " / 0 (expect error): " + (passed ? "PASSED" : "FAILED");

        if(passed) {
            logger_->info(result);
        } else {
            logger_->error(result);
        }

        return passed;
    }

    // Тест статистики
    bool testStatistics() {
        calculator::StatisticsRequest request;
        calculator::StatisticsResponse response;
        grpc::ClientContext context;

        auto status = stub_->GetStatistics(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        logger_->info("=== Statistics ===");
        logger_->info("  Total calculations: " + std::to_string(response.total_calculations()));
        logger_->info("  Cache hits: " + std::to_string(response.cache_hits()));
        logger_->info("  Database hits: " + std::to_string(response.database_hits()));
        logger_->info("  Cache size: " + std::to_string(response.cache_size()));
        logger_->info("  Cache hit rate: " + std::to_string(response.cache_hit_rate()) + "%");
        logger_->info("  Database hit rate: " + std::to_string(response.database_hit_rate()) + "%");

        return true;
    }

    // Тест здоровья
    bool testHealth() {
        calculator::HealthRequest request;
        calculator::HealthResponse response;
        grpc::ClientContext context;

        auto status = stub_->HealthCheck(&context, request, &response);

        if(!status.ok()) {
            logger_->error("RPC failed: " + status.error_message());
            return false;
        }

        logger_->info("=== Health Check ===");
        logger_->info(std::string("  Status: ") + (response.healthy() ? "HEALTHY" : "UNHEALTHY"));
        logger_->info("  Message: " + response.status());
        logger_->info("  Uptime: " + std::to_string(response.uptime_seconds()) + " seconds");

        return response.healthy();
    }

   private:
    std::unique_ptr<calculator::Calculator::Stub> stub_;
    std::shared_ptr<Logger> logger_;
};

// Функция ожидания с таймаутом (без sleep)
bool waitForServer(const std::string& server_address, int timeout_seconds, std::shared_ptr<Logger> logger) {
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    auto stub = calculator::Calculator::NewStub(channel);

    auto start = std::chrono::steady_clock::now();
    int attempt = 0;

    while(std::chrono::steady_clock::now() - start < std::chrono::seconds(timeout_seconds)) {
        calculator::HealthRequest request;
        calculator::HealthResponse response;
        grpc::ClientContext context;

        auto status = stub->HealthCheck(&context, request, &response);
        if(status.ok()) {
            logger->info("Server is available after " + std::to_string(attempt) + " attempts");
            return true;
        }

        attempt++;

        // Активное ожидание без sleep
        auto start = std::chrono::steady_clock::now();
        while(std::chrono::steady_clock::now() - start < std::chrono::microseconds(100)) {
        }  // пусто - активное ожидание
    }

    logger->error("Server not available after " + std::to_string(timeout_seconds) + " seconds");
    return false;
}

int main(int argc, char** argv) {
    // Создаем логгер
    auto logger = Logger::create();
    logger->info("Starting calculator client tests");

    std::string server_address = "localhost:50051";

    // Можно передать адрес сервера как аргумент
    if(argc > 1) {
        server_address = argv[1];
        logger->info("Using custom server address: " + server_address);
    } else {
        logger->info("Using default server address: " + server_address);
    }

    // Ждем сервер с таймаутом 5 секунд
    if(!waitForServer(server_address, 5, logger)) {
        logger->error("Server not available, exiting");
        return 1;
    }

    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    CalculatorClient client(channel, logger);

    int passed = 0;
    int total = 0;

    logger->info("=== Starting Calculator Tests ===");

    // Базовые тесты
    logger->info("\n--- Basic Arithmetic Tests ---");
    passed += client.testAddition(5, 3, 8);
    total++;
    passed += client.testAddition(-2, 7, 5);
    total++;
    passed += client.testSubtraction(10, 4, 6);
    total++;
    passed += client.testSubtraction(1, 5, -4);
    total++;
    passed += client.testMultiplication(6, 7, 42);
    total++;
    passed += client.testMultiplication(-3, 4, -12);
    total++;
    passed += client.testDivision(15, 3, 5);
    total++;
    passed += client.testDivision(7, 2, 3);
    total++;  // Целочисленное деление

    logger->info("\n--- Factorial Tests ---");
    passed += client.testFactorial(0, 1);
    total++;
    passed += client.testFactorial(1, 1);
    total++;
    passed += client.testFactorial(5, 120);
    total++;

    logger->info("\n--- Error Tests ---");
    passed += client.testDivisionByZero(10);
    total++;

    // Дополнительные проверки
    client.testStatistics();
    client.testHealth();

    logger->info("\n=== Test Summary ===");
    std::string summary = "Passed: " + std::to_string(passed) + "/" + std::to_string(total);

    if(passed == total) {
        logger->info("✅ " + summary + " - ALL TESTS PASSED");
    } else {
        logger->error("❌ " + summary + " - SOME TESTS FAILED");
    }

    return (passed == total) ? 0 : 1;
}