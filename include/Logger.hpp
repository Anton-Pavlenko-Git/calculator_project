#pragma once

#include <memory>
#include <string>

class Logger {
   public:
    // Виртуальный деструктор для полиморфизма
    virtual ~Logger() = default;

    // Запрещаем копирование и перемещение для интерфейса
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // Интерфейс логирования
    virtual void info(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    virtual void debug(const std::string& message) = 0;

    // Фабричный метод для создания реального логгера (spdlog)
    static std::shared_ptr<Logger> create();

   protected:
    // Защищенный конструктор по умолчанию
    Logger() = default;
};

// Реализация для spdlog
class SpdLogger : public Logger {
   public:
    SpdLogger();
    ~SpdLogger() override;

    SpdLogger(const SpdLogger&) = delete;
    SpdLogger& operator=(const SpdLogger&) = delete;
    SpdLogger(SpdLogger&&) = delete;
    SpdLogger& operator=(SpdLogger&&) = delete;

    void info(const std::string& message) override;
    void error(const std::string& message) override;
    void debug(const std::string& message) override;
};