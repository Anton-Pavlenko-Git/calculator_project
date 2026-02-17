#pragma once

#include <grpcpp/grpcpp.h>

#include <chrono>

#include "CalculatorEngine.hpp"
#include "Logger.hpp"
#include "calculator.grpc.pb.h"

namespace calculator {

// Реализация gRPC сервиса калькулятора
class CalculatorServiceImpl final : public calculator::Calculator::Service {
   public:
    /*
     * Конструктор
     * logger Логгер для записи событий
     * engine Движок калькулятора
     */
    CalculatorServiceImpl(std::shared_ptr<Logger> logger, std::unique_ptr<CalculatorEngine> engine);

    // Обработчик RPC вызова Calculate
    grpc::Status Calculate(
        grpc::ServerContext* context, const calculator::CalculationRequest* request,
        calculator::CalculationResponse* response
    ) override;

    // Обработчик RPC вызова GetStatistics
    grpc::Status GetStatistics(
        grpc::ServerContext* context, const calculator::StatisticsRequest* request,
        calculator::StatisticsResponse* response
    ) override;

    // Обработчик RPC вызова HealthCheck
    grpc::Status HealthCheck(
        grpc::ServerContext* context, const calculator::HealthRequest* request, calculator::HealthResponse* response
    ) override;

   private:
    std::shared_ptr<Logger> logger_{nullptr};
    std::unique_ptr<CalculatorEngine> engine_{nullptr};
    std::chrono::steady_clock::time_point start_time_{};

    // Конвертация строковой операции в enum
    std::string convertOperation(const std::string& operation);
};

}  // namespace calculator