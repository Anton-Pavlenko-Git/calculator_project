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
    void processInput(const std::string& jsonInput);

    // Вспомогательный метод
    static void printHelp();
};