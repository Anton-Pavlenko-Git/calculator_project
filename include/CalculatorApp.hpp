#pragma once

#include <memory>
#include <string>

#include "CalculatorEngine.hpp"
#include "Logger.hpp"

class CalculatorApp {
   private:
    std::shared_ptr<Logger> logger_;
    CalculatorEngine engine_;

   public:
    // Констуктор с внедрением зависимости
    explicit CalculatorApp(std::shared_ptr<Logger> logger);

    // Основной метод запуска приложения
    void run();

    // Обработка одного вычисления
    std::string calculate(const std::string& jsonInput);
    void processInput(const std::string& jsonInput);

    // Вспомогательный метод
    static void printHelp() noexcept;

    ~CalculatorApp() noexcept = default;
    CalculatorApp(const CalculatorApp&) noexcept = default;
    CalculatorApp& operator=(const CalculatorApp&) noexcept = default;
    CalculatorApp(CalculatorApp&&) noexcept = default;
    CalculatorApp& operator=(CalculatorApp&&) noexcept = default;
};