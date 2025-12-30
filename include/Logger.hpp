#pragma once

#include <memory>
#include <string>

class Logger {
   public:
    // Виртуальный деструктор для полиморфизма
    virtual ~Logger() noexcept = default;

    // Запрещаем копирование и перемещение для интерфейса
    Logger(const Logger&) noexcept = delete;
    Logger& operator=(const Logger&) noexcept = delete;
    Logger(Logger&&) noexcept = delete;
    Logger& operator=(Logger&&) noexcept = delete;

    // Интерфейс логирования
    virtual void info(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    virtual void debug(const std::string& message) = 0;

    // Фабричный метод для создания реального логгера (spdlog)
    static std::shared_ptr<Logger> create();

   protected:
    // Защищенный конструктор по умолчанию
    Logger() noexcept = default;
};

// Реализация для spdlog
class SpdLogger : public Logger {
   public:
    SpdLogger();
    ~SpdLogger() override;

    SpdLogger(const SpdLogger&) noexcept = delete;
    SpdLogger& operator=(const SpdLogger&) noexcept = delete;
    SpdLogger(SpdLogger&&) noexcept = delete;
    SpdLogger& operator=(SpdLogger&&) noexcept = delete;

    void info(const std::string& message) override;
    void error(const std::string& message) override;
    void debug(const std::string& message) override;
};