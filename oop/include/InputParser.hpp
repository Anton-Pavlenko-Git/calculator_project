#pragma once

#include <string>

#include "CalculationRequest.hpp"

class InputParser {
   public:
    // Основной метод: принимает JSON, возвращает готовый CalculationRequest
    // Может бросать исключения при ошибках парсинга
    CalculationRequest parseFromJson(const std::string& jsonStr) const;
    bool validateJson(const std::string& jsonStr) const;

    // Вспомогательный метод для тестов
    static CalculationRequest parseFromString(const std::string& input);
};