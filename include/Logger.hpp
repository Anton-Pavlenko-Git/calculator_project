#pragma once

#include <memory>
#include <string>

class Logger {
   public:
    // Виртуальный деструктор для полиморфизма
    virtual ~Logger() = default;

    // Интерфейс логирования
    virtual void info(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    virtual void debug(const std::string& message) = 0;

    // Фабричный метод для создания реального логгера (spdlog)
    static std::shared_ptr<Logger> create();
};

// Реализация для spdlog
class SpdLogger : public Logger {
   public:
    SpdLogger();
    void info(const std::string& message) override;
    void error(const std::string& message) override;
    void debug(const std::string& message) override;
    ~SpdLogger() override;
};