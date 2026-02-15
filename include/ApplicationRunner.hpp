#pragma once

#include "Logger.hpp"

namespace app {

bool isRunningUnderSystemd();

// Запуск приложения в нужном режиме
class ApplicationRunner {
   public:
    // Парсинг аргументов командной строки
    struct Config {
        bool run_as_daemon = false;
        bool run_grpc_server = false;
        std::string grpc_address = "0.0.0.0:50051";
        bool show_help = false;
    };

    // Парсинг аргументов командной строки
    static Config parseArguments(int argc, char* argv[]);

    // Показать справку
    static void showHelp(const char* program_name) noexcept;

    // Запуск приложения
    static int run(const Config& config);
};

}  // namespace app