#include <iostream>

#include "CalculatorApp.hpp"

int main() {
    try {
        // 1. Создаем логгер
        auto logger = Logger::create();

        // Всегда используем БД (для демонстрации работы)
        std::string connStr = "host=localhost dbname=calculator_db user=anton password=passWord";

        // 2. Создаем приложение с внедренным логгером
        CalculatorApp app(logger, connStr);

        // 3. Запускаем приложение
        app.run();

        return 0;

    } catch(const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    } catch(...) {
        std::cerr << "Fatal error: Unknown exception occurred\n";
        return 1;
    }
}