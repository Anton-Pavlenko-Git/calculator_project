#pragma once

#include "CalculationRequest.hpp"

class CalculatorEngine {
   public:
    int calculate(const CalculationRequest& request) const;

    CalculatorEngine() noexcept = default;
    ~CalculatorEngine() noexcept = default;

    CalculatorEngine(const CalculatorEngine&) noexcept = default;
    CalculatorEngine& operator=(const CalculatorEngine&) noexcept = default;
    CalculatorEngine(CalculatorEngine&&) noexcept = default;
    CalculatorEngine& operator=(CalculatorEngine&&) noexcept = default;
};
