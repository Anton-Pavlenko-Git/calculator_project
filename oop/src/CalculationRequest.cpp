#include "CalculationRequest.hpp"

#include <nlohmann/json.hpp>

#include "CalculatorExceptions.hpp"

using json = nlohmann::json;

CalculationRequest::CalculationRequest(int op1, std::string op, std::optional<int> op2)
    : operand1_(op1), operation_(std::move(op)), operand2_(op2) {}

// Парсинг из JSON строки
CalculationRequest CalculationRequest::fromJson(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        return fromJson(j);
    } catch(const json::parse_error& e) {
        throw InvalidInputException("Invalid JSON: " + std::string(e.what()));
    }
}

// Парсинг из объекта json
CalculationRequest CalculationRequest::fromJson(const nlohmann::json& j) {
    // Проверяем обязательные поля
    if(!j.contains("operand1") || !j.contains("operation")) {
        throw InvalidInputException("Missing required fields: operand1 and operation");
    }

    int operand1 = j["operand1"].get<int>();
    std::string operation = j["operation"].get<std::string>();

    // operand2 может отсутствовать (для факториала)
    std::optional<int> operand2 = std::nullopt;
    if(j.contains("operand2")) {
        operand2 = j["operand2"].get<int>();
    }

    return CalculationRequest(operand1, operation, operand2);
}

// Преобразуем в JSON строку
std::string CalculationRequest::toJson() const {
    return toJsonObject().dump(2);  // dump(2) для красивого форматирования
}

// Преобразуем в объект json
nlohmann::json CalculationRequest::toJsonObject() const {
    json j;
    j["operand1"] = operand1_;
    j["operation"] = operation_;

    if(operand2_.has_value()) {
        j["operand2"] = operand2_.value();
    }

    return j;
}