#pragma once

#include <grpcpp/grpcpp.h>

#include <atomic>
#include <thread>

#include "CalculatorEngine.hpp"
#include "CalculatorServiceImpl.hpp"
#include "Logger.hpp"

namespace calculator {

// Управление gRPC сервером
class GrpcServer {
   public:
    /*
     * Конструктор
     * logger Логгер
     * engine Движок калькулятора
     * address Адрес для прослушивания (например, "0.0.0.0:50051")
     */
    GrpcServer(
        std::shared_ptr<Logger> logger, std::unique_ptr<CalculatorEngine> engine,
        const std::string& address = "0.0.0.0:50051"
    );

    ~GrpcServer();

    // Удаляем конструкторы копирования и присваивания
    GrpcServer(const GrpcServer&) = delete;
    GrpcServer& operator=(const GrpcServer&) = delete;
    GrpcServer(GrpcServer&&) = delete;
    GrpcServer& operator=(GrpcServer&&) = delete;

    /*
     * Запуск сервера
     * std::runtime_error если не удалось запустить
     */
    void start();

    /*
     * Остановка сервера
     * graceful Корректная остановка (дожидается завершения запросов)
     */
    void stop(bool graceful = true);

    // Проверка, запущен ли сервер
    bool isRunning() const;

    // Получение адреса сервера
    std::string getAddress() const noexcept;

   private:
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<CalculatorEngine> engine_;
    std::string address_;
    std::atomic<bool> running_{false};
    std::unique_ptr<grpc::Server> server_;
    std::unique_ptr<CalculatorServiceImpl> service_;
};
}  // namespace calculator