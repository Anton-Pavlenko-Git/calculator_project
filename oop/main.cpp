#include <iostream>
#include <memory>

#include "include/CalculatorApp.hpp"
#include "include/Logger.hpp"

int main(int argc, char* argv[]) {
    try {
        // 1. Создаем логгер
        auto logger = Logger::create();

        // 2. Создаем приложение с внедренным логгером
        CalculatorApp app(logger);

        // 3. Запускаем приложение
        app.run();

        return 0;

    } catch(const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
