#pragma once

#include "CalculationRequest.hpp"

class CalculatorEngine {
   public:
    int calculate(const CalculationRequest& request) const;

    // Правило пяти
    CalculatorEngine() = default;
    ~CalculatorEngine() = default;

    CalculatorEngine(const CalculatorEngine&) = default;
    CalculatorEngine& operator=(const CalculatorEngine&) = default;
    CalculatorEngine(CalculatorEngine&&) = default;
    CalculatorEngine& operator=(CalculatorEngine&&) = default;
};
