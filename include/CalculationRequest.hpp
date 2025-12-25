#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

class CalculationRequest {
   private:
    int operand1_;
    std::string operation_;
    std::optional<int> operand2_;

   public:
    CalculationRequest(int op1, std::string oper, std::optional<int> op2 = std::nullopt);

    [[nodiscard]] int getFirstOperand() const { return operand1_; }
    [[nodiscard]] std::string getOperation() const { return operation_; }
    [[nodiscard]] std::optional<int> getSecondOperand() const { return operand2_; }

    static CalculationRequest fromJson(const std::string& jsonStr);
    static CalculationRequest fromJson(const nlohmann::json& jsonObj);
    [[nodiscard]] std::string toJson() const;
    [[nodiscard]] nlohmann::json toJsonObject() const;
};