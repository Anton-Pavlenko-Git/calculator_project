#pragma once

#include <memory>
#include <string>

#include "CalculatorEngine.hpp"
#include "InputParser.hpp"
#include "Logger.hpp"

class CalculatorApp {
   private:
    std::shared_ptr<Logger> logger_;
    InputParser parser_;
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

    // private:
    //  Приватные методы для внутренней организации
    // void processCalculation(const CalculationRequest& request);
};