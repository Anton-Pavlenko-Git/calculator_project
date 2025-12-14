#include "InputParser.hpp"

CalculationRequest InputParser::parseFromJson(const std::string& jsonStr) const {
    // TODO: реализовать логику парсинга
    return CalculationRequest(0, "+", 0);
}

bool InputParser::validateJson(const std::string& jsonStr) const {
    // TODO: логика валидации
    return !jsonStr.empty();
}

CalculationRequest InputParser::parseFromString(const std::string& input) {
    // TODO: логика для тестов
    return CalculationRequest(0, "+", 0);
}