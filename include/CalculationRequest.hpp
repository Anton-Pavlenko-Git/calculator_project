#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

class CalculationRequest {
   private:
    int operand1_;
    std::string operation_;
    std::optional<int> operand2_;

    bool cached_ = false;
    bool from_database_ = false;

   public:
    explicit CalculationRequest(int op1, std::string oper, std::optional<int> op2 = std::nullopt) noexcept;

    [[nodiscard]] int getFirstOperand() const noexcept;
    [[nodiscard]] std::string getOperation() const noexcept;
    [[nodiscard]] std::optional<int> getSecondOperand() const noexcept;

    static CalculationRequest fromJson(const std::string& jsonStr);
    static CalculationRequest fromJson(const nlohmann::json& jsonObj);
    [[nodiscard]] std::string toJson() const;
    [[nodiscard]] nlohmann::json toJsonObject() const;

    ~CalculationRequest() noexcept = default;
    CalculationRequest(const CalculationRequest&) noexcept = default;
    CalculationRequest& operator=(const CalculationRequest&) noexcept = default;
    CalculationRequest(CalculationRequest&&) noexcept = default;
    CalculationRequest& operator=(CalculationRequest&&) noexcept = default;

    bool wasCached() const noexcept { return cached_; }
    bool wasFromDatabase() const noexcept { return from_database_; }
    void setCached(bool cached) noexcept { cached_ = cached; }
    void setFromDatabase(bool from_db) noexcept { from_database_ = from_db; }
};