#include "ApplicationRunner.hpp"

int main(int argc, char* argv[]) {
    // 1. Парсим аргументы
    auto config = app::ApplicationRunner::parseArguments(argc, argv);

    // 2. Показываем справку если нужно
    if(config.show_help) {
        app::ApplicationRunner::showHelp(argv[0]);
        return 0;
    }

    // 3. Запускаем приложение
    return app::ApplicationRunner::run(config);
}