#pragma once

#include "CalculationRequest.hpp"

class CalculatorEngine {
   public:
    int calculate(const CalculationRequest& request) const;
};