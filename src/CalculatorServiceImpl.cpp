#include "CalculatorServiceImpl.hpp"

#include <chrono>

#include "CalculationRequest.hpp"
#include "CalculatorExceptions.hpp"

namespace calculator {

CalculatorServiceImpl::CalculatorServiceImpl(std::shared_ptr<Logger> logger, std::unique_ptr<CalculatorEngine> engine)
    : logger_{std::move(logger)}, engine_{std::move(engine)}, start_time_{std::chrono::steady_clock::now()} {
    if(!logger_) {
        throw std::invalid_argument("Logger cannot be null");
    }

    if(!engine_) {
        throw std::invalid_argument("CalculatorEngine cannot be null");
    }

    logger_->info("CalculatorServiceImpl created");
}

grpc::Status CalculatorServiceImpl::Calculate(
    grpc::ServerContext* context, const calculator::CalculationRequest* request,
    calculator::CalculationResponse* response
) {
    // Логируем входящий запрос
    logger_->debug("Calculate RPC called from: " + context->peer());
    logger_->debug(
        "Request: " + std::to_string(request->operand1()) + " " + request->operation() + " " +
        std::to_string(request->operand2())
    );

    try {
        // Конвертируем protobuf запрос в CalculationRequest
        ::CalculationRequest calc_request(
            request->operand1(), convertOperation(request->operation()), request->operand2()
        );

        // Выполняем вычисление
        int result = engine_->calculate(calc_request);

        // Заполняем ответ
        response->set_result(result);
        response->set_from_cache(calc_request.wasCached());
        response->set_from_database(calc_request.wasFromDatabase());

        logger_->info("Calculate successful: " + std::to_string(result));
        return grpc::Status::OK;

    } catch(const DivisionByZeroException& e) {
        logger_->error("Division by zero: " + std::string(e.what()));
        response->set_error(e.what());
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());

    } catch(const NegativeFactorialException& e) {
        logger_->error("Negative factorial: " + std::string(e.what()));
        response->set_error(e.what());
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());

    } catch(const InvalidOperationException& e) {
        logger_->error("Invalid operation: " + std::string(e.what()));
        response->set_error(e.what());
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());

    } catch(const InvalidInputException& e) {
        logger_->error("Invalid input: " + std::string(e.what()));
        response->set_error(e.what());
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());

    } catch(const MathException& e) {
        logger_->error("Math error: " + std::string(e.what()));
        response->set_error(e.what());
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());

    } catch(const std::exception& e) {
        // Внутренняя ошибка сервера
        logger_->error("Internal error in Calculate: " + std::string(e.what()));
        response->set_error("Internal server error");
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal server error");
    }
}

grpc::Status CalculatorServiceImpl::GetStatistics(
    grpc::ServerContext* context, [[maybe_unused]] const calculator::StatisticsRequest* request,
    calculator::StatisticsResponse* response
) {
    logger_->debug("GetStatistics RPC called from: " + context->peer());

    try {
        auto stats = engine_->getStats();

        response->set_total_calculations(stats.calculations);
        response->set_cache_hits(stats.cacheHits);
        response->set_database_hits(stats.dbHits);
        response->set_cache_size(stats.cacheSize);
        response->set_cache_hit_rate(stats.cacheHitRate());
        response->set_database_hit_rate(stats.dbHitRate());

        return grpc::Status::OK;

    } catch(const std::exception& e) {
        logger_->error("Error in GetStatistics: " + std::string(e.what()));
        return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to get statistics");
    }
}

grpc::Status CalculatorServiceImpl::HealthCheck(
    [[maybe_unused]] grpc::ServerContext* context, [[maybe_unused]] const calculator::HealthRequest* request,
    calculator::HealthResponse* response
) {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_);

    response->set_healthy(true);
    response->set_status("OK");
    response->set_uptime_seconds(uptime.count());

    return grpc::Status::OK;
}

std::string CalculatorServiceImpl::convertOperation(const std::string& operation) {
    if(operation.empty()) {
        throw InvalidInputException("Operation cannot be empty");
    }

    // Берем первый символ операции
    char op = operation[0];

    // Проверяем допустимые операции
    const std::string valid_ops = "+-*/^!";
    if(valid_ops.find(op) == std::string::npos) {
        throw InvalidOperationException("Invalid operation: " + operation);
    }

    return std::string(1, op);
}

}  // namespace calculator