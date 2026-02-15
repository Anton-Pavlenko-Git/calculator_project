#include "ApplicationRunner.hpp"

#include <unistd.h>

#include <fstream>
#include <iostream>

#include "CalculatorEngine.hpp"
#include "Daemon.hpp"
#include "GrpcServer.hpp"
#include "Logger.hpp"

namespace app {

bool isRunningUnderSystemd() {
    // Проверяем, запущены ли мы под systemd
    return getppid() == 1 ||                      // Родитель - init (PID 1)
           getenv("INVOCATION_ID") != nullptr ||  // systemd переменная
           getenv("JOURNAL_STREAM") != nullptr;   // systemd переменная
}

ApplicationRunner::Config ApplicationRunner::parseArguments(int argc, char* argv[]) {
    Config config;

    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if(arg == "--daemon" || arg == "-d") {
            config.run_as_daemon = true;
        } else if(arg == "--grpc" || arg == "-g") {
            config.run_grpc_server = true;
        } else if(arg == "--address" && i + 1 < argc) {
            config.grpc_address = argv[++i];
        } else if(arg == "--help" || arg == "-h") {
            config.show_help = true;
        }
    }

    return config;
}

void ApplicationRunner::showHelp(const char* program_name) {
    std::cout << "Calculator gRPC Service\n"
              << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  -d, --daemon      Run as daemon\n"
              << "  -g, --grpc        Run gRPC server\n"
              << "  --address ADDR    gRPC server address (default: 0.0.0.0:50051)\n"
              << "  -h, --help        Show this help\n";
}

int ApplicationRunner::run(const Config& config) {
    try {
        auto logger = Logger::create();
        std::string connStr = "host=localhost dbname=calculator_db user=anton password=passWord";

        // Демонизация:
        // - Не вызываем Daemon::daemonize() при запуске через systemd
        // - Для ручного запуска с --daemon нужно раскомментировать вызов
        if(config.run_as_daemon && !isRunningUnderSystemd()) {
            logger->info("Running as daemon");
            // Daemon::daemonize();

            // Замисываем PID
            std::ofstream pid_file("/var/run/calculator.pid");
            if(pid_file) {
                pid_file << getpid() << std::endl;
            }
            logger->info("Daemon started with PID: " + std::to_string(getpid()));
        }

        // Запуск gRPC сервера
        if(config.run_grpc_server) {
            // Режим gRPC сервера
            logger->info("Starting gRPC server on " + config.grpc_address);

            // Создаем движок калькулятора
            auto engine = std::make_unique<CalculatorEngine>(logger, connStr);

            // Создаем и запускаем gRPC сервер
            calculator::GrpcServer server(logger, std::move(engine), config.grpc_address);

            server.start();

            // Обработка сигналов
            Daemon daemon(logger);
            daemon.start();

            if(!config.run_as_daemon) {
                std::cout << "Calculator gRPC server running on " << config.grpc_address << std::endl;
                std::cout << "Press Ctrl+C to stop..." << std::endl;
            } else {
                logger->info("Running in daemon mode, entering main loop");
            }

            // Ожидание сигнала остановки
            logger->info("Server is running, waiting for shutdown signal");
            while(daemon.isRunning() && server.isRunning()) {
                pause();  // Блокируется до получения сигнала
            }

            // Корректная остановка
            logger->info("Shutting down gRPC server...");
            server.stop();
            logger->info("gRPC server stopped");

        } else {
            // Режим по умолчанию - сообщение
            if(!config.run_as_daemon) {
                showHelp("calculator");
            } else {
                logger->info("No mode specified, running as idle daemon");
                Daemon daemon(logger);
                daemon.start();

                while(daemon.isRunning()) {
                    pause();
                }
            }
        }

        logger->info("Service stopped");
        return 0;

    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch(...) {
        std::cerr << "Fatal error: unknown exception" << std::endl;
        return 1;
    }
}

}  // namespace app