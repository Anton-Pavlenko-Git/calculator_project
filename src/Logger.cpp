#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

// Фабричный метод
std::shared_ptr<Logger> Logger::create() { return std::make_shared<SpdLogger>(); }

// Коструктор SpdLogger
SpdLogger::SpdLogger() {
    // Создаем логгер с цветным выводом в консоль
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("calculator", console_sink);

    // Настройка формата: [время] [уровень] сообщение
    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

    // Уровень логирования (debug, info, error)
    logger->set_level(spdlog::level::debug);

    spdlog::set_default_logger(logger);
}

// Методы SpdLogger

void SpdLogger::info(const std::string& message) { spdlog::info(message); }

void SpdLogger::error(const std::string& message) { spdlog::error(message); }

void SpdLogger::debug(const std::string& message) { spdlog::debug(message); }

SpdLogger::~SpdLogger() { spdlog::shutdown(); }