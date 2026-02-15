#include "GrpcServer.hpp"

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>

namespace calculator {

GrpcServer::GrpcServer(
    std::shared_ptr<Logger> logger, std::unique_ptr<CalculatorEngine> engine, const std::string& address
)
    : logger_(std::move(logger)), engine_(std::move(engine)), address_(address) {
    if(!logger_) {
        throw std::invalid_argument("Logger cannot be null");
    }

    if(!engine_) {
        throw std::invalid_argument("CalculatorEngine cannot be null");
    }

    if(address_.empty()) {
        throw std::invalid_argument("Address cannot be empty");
    }

    logger_->info("GrpcServer created for address: " + address_);
}

GrpcServer::~GrpcServer() {
    // Гарантируем корректную остановку в деструкторе
    if(running_.load()) {
        stop(false);  // Форсированная остановка в деструкторе
    }
}

void GrpcServer::start() {
    if(running_.exchange(true)) {
        logger_->info("Server is already running");
        return;
    }

    logger_->info("Starting gRPC server on " + address_);

    try {
        // Создаем сервис
        auto service = std::make_unique<CalculatorServiceImpl>(logger_, std::move(engine_));

        // Сохраняем сервис (должен жить дольше сервера)
        // Внимание: unique_ptr передается сервису, который теперь владеет им
        service_ = std::move(service);

        // Создаем builder и настраиваем сервер
        grpc::ServerBuilder builder;

        // Добавляем порт для прослушивания
        builder.AddListeningPort(address_, grpc::InsecureServerCredentials());

        // Регистрируем сервис
        builder.RegisterService(service_.get());

        // Настраиваем количество потоков
        // MIN_POLLERS - минимальное количество потоков для обработки RPC
        // MAX_POLLERS - максимальное количество потоков
        builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MIN_POLLERS, 2);
        builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, 4);

        // Настраиваем максимальное количество сообщений в очереди
        builder.SetMaxReceiveMessageSize(10 * 1024 * 1024);  // 10MB
        builder.SetMaxSendMessageSize(10 * 1024 * 1024);     // 10MB

        // Создаем и запускаем сервер
        server_ = builder.BuildAndStart();

        if(!server_) {
            throw std::runtime_error("Failed to start gRPC server");
        }

        logger_->info("gRPC server started successfully on " + address_);
        logger_->info("Server is ready to accept connections");

    } catch(const std::exception& e) {
        running_.store(false);
        logger_->error("Failed to start gRPC server: " + std::string(e.what()));
        throw std::runtime_error("Failed to start gRPC server: " + std::string(e.what()));
    }
}

void GrpcServer::stop(bool graceful) {
    if(!running_.exchange(false)) {
        return;
    }

    logger_->info("Stopping gRPC server" + std::string(graceful ? " (graceful)" : " (forced)"));

    if(server_) {
        if(graceful) {
            // Корректная остановка: ждем завершения текущих запросов
            // Устанавливаем дедлайн 10 секунд на graceful shutdown
            auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(10);
            server_->Shutdown(deadline);

            logger_->debug("Graceful shutdown initiated, waiting up to 10 seconds");
        } else {
            // Немедленная остановка
            server_->Shutdown();
            logger_->debug("Forced shutdown initiated");
        }

        // Ждем завершения работы сервера
        // Это блокирующий вызов, но он нормален при остановке
        server_->Wait();

        // Освобождаем ресурсы
        server_.reset();
        service_.reset();

        logger_->info("gRPC server stopped");
    } else {
        logger_->info("Server was not running");
    }
}

bool GrpcServer::isRunning() const noexcept { return running_.load() && server_ != nullptr; }

std::string GrpcServer::getAddress() const noexcept { return address_; }

}  // namespace calculator