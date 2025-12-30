#include "CalculationRequest.hpp"

#include <nlohmann/json.hpp>

#include "CalculatorExceptions.hpp"

using json = nlohmann::json;

// Конструктор
CalculationRequest::CalculationRequest(int op1, std::string oper, std::optional<int> op2) noexcept
    : operand1_(op1), operation_(std::move(oper)), operand2_(op2) {}

int CalculationRequest::getFirstOperand() const noexcept { return operand1_; }
std::string CalculationRequest::getOperation() const noexcept { return operation_; }
std::optional<int> CalculationRequest::getSecondOperand() const noexcept { return operand2_; }

// Парсинг из JSON строки
CalculationRequest CalculationRequest::fromJson(const std::string& jsonStr) {
    try {
        json const jay = json::parse(jsonStr);
        return fromJson(jay);
    } catch(const json::parse_error& e) {
        throw InvalidInputException("Invalid JSON: " + std::string(e.what()));
    }
}

// Парсинг из объекта json
CalculationRequest CalculationRequest::fromJson(const nlohmann::json& jsonObj) {
    // Проверяем обязательные поля
    if(!jsonObj.contains("operand1") || !jsonObj.contains("operation")) {
        throw InvalidInputException("Missing required fields: operand1 and operation");
    }

    int const operand1 = jsonObj["operand1"].get<int>();
    std::string const operation = jsonObj["operation"].get<std::string>();

    // operand2 может отсутствовать (для факториала)
    std::optional<int> operand2 = std::nullopt;
    if(jsonObj.contains("operand2")) {
        operand2 = jsonObj["operand2"].get<int>();
    }

    return CalculationRequest{operand1, operation, operand2};
}

// Преобразуем в JSON строку
std::string CalculationRequest::toJson() const {
    return toJsonObject().dump(2);  // dump(2) для красивого форматирования
}

// Преобразуем в объект json
nlohmann::json CalculationRequest::toJsonObject() const {
    json jay;
    jay["operand1"] = operand1_;
    jay["operation"] = operation_;

    if(operand2_.has_value()) {
        jay["operand2"] = operand2_.value();
    }

    return jay;
}