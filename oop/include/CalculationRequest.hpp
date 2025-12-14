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
    CalculationRequest(int op1, std::string op, std::optional<int> op2 = std::nullopt);

    int getFirstOperand() const { return operand1_; }
    std::string getOperation() const { return operation_; }
    std::optional<int> getSecondOperand() const { return operand2_; }

    static CalculationRequest fromJson(const std::string& jsonStr);
    static CalculationRequest fromJson(const nlohmann::json& j);
    std::string toJson() const;
    nlohmann::json toJsonObject() const;
};