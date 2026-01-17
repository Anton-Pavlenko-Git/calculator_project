#pragma once

// #include <memory>
#include <string>

#include "CalculatorEngine.hpp"
#include "Logger.hpp"

class CalculatorApp {
   private:
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<CalculatorEngine> engine_;

   public:
    // Констуктор с внедрением зависимости
    explicit CalculatorApp(std::shared_ptr<Logger> logger);

    CalculatorApp(std::shared_ptr<Logger> logger, const std::string& databaseConnectionString);

    // Основной метод запуска приложения
    void run();

    // Обработка одного вычисления
    std::string calculate(const std::string& jsonInput);
    void processInput(const std::string& jsonInput);

    // Вспомогательный метод
    static void printHelp() noexcept;
    void printStats() const;

    ~CalculatorApp() noexcept = default;
    CalculatorApp(const CalculatorApp&) noexcept = delete;
    CalculatorApp& operator=(const CalculatorApp&) noexcept = delete;
    CalculatorApp(CalculatorApp&&) noexcept = default;
    CalculatorApp& operator=(CalculatorApp&&) noexcept = default;
};