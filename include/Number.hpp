#pragma once

// #include <optional>
#include <string>

struct Number {
    int first{0};
    int second{0};  // 0 для унарных операций
    std::string operation;
    int result{0};

    // Конструкторы для удобства
    Number() = default;

    // Для бинарных операций
    Number(int first, int second, std::string operation, int result)
        : first(first), second(second), operation(std::move(operation)), result(result) {}

    // Для унарных операций (факториал)
    Number(int first, std::string operation, int result)
        : first(first), operation(std::move(operation)), result(result) {}
};