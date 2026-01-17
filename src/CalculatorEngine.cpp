#include "CalculatorEngine.hpp"

#include <cmath>
#include <iostream>
#include <math_utils/math_utils.hpp>

#include "CalculationRequest.hpp"
#include "CalculatorExceptions.hpp"

// ========== Конструкторы ==========
CalculatorEngine::CalculatorEngine() : cache_(nullptr), database_(nullptr) {}

CalculatorEngine::CalculatorEngine(const std::string& connectionString)
    : cache_(std::make_unique<CalculationCache>()), database_(std::make_unique<Database>(connectionString)) {
    warmCache();
}

CalculatorEngine::CalculatorEngine(CalculatorEngine&& other) noexcept
    : cache_(std::move(other.cache_))
    , database_(std::move(other.database_))
    , cacheHits_(other.cacheHits_)
    , dbHits_(other.dbHits_)
    , calculations_(other.calculations_) {
    other.cacheHits_ = 0;
    other.dbHits_ = 0;
    other.calculations_ = 0;
}

CalculatorEngine& CalculatorEngine::operator=(CalculatorEngine&& other) noexcept {
    if(this != &other) {
        cache_ = std::move(other.cache_);
        database_ = std::move(other.database_);
        cacheHits_ = other.cacheHits_;
        dbHits_ = other.dbHits_;
        calculations_ = other.calculations_;

        other.cacheHits_ = 0;
        other.dbHits_ = 0;
        other.calculations_ = 0;
    }
    return *this;
}

// ========== Основной публичный метод (минимальные изменения) ==========
int CalculatorEngine::calculate(const CalculationRequest& request) const {
    bool isUnary = (request.getOperation() == "!" || request.getOperation() == "fact");

    // Делегируем приватному методу с кешированием
    if(isUnary) {
        // Для факториала
        return calculateWithCache(request.getFirstOperand(), request.getOperation(), std::nullopt);
    }
    // Для бинарных операций
    auto operand2 = request.getSecondOperand();
    if(!operand2) {
        throw std::logic_error("Binary operation must have operand2");
    }
    return calculateWithCache(request.getFirstOperand(), request.getOperation(), operand2);
}

// ========== Приватный метод с логикой кеширования ==========
int CalculatorEngine::calculateWithCache(
    int operand1, const std::string& operation, std::optional<int> operand2
) const {
    calculations_++;

    // 1. Проверяем кеш (если есть)
    if(cache_) {
        auto cachedResult = cache_->get(operand1, operation, operand2);
        if(cachedResult) {
            cacheHits_++;
            return *cachedResult;
        }
    }

    // 2. Проверяем БД (если есть и не унарная операция)
    if(database_) {
        auto dbResult = database_->findCalculation(operand1, operation, operand2);
        if(dbResult) {
            dbHits_++;

            // Сохраняем в кеш
            if(cache_) {
                cache_->put(operand1, operation, operand2, *dbResult);
            }

            return *dbResult;
        }
    }

    // 3. Вычисляем
    int result = 0;

    if(operation == "!") {
        result = calculateFactorialDirectly(operand1);
    } else {
        // operand2 гарантированно имеет значение для бинарных операций
        result = calculateDirectly(operand1, operation, operand2.value());
    }

    // 4. Сохраняем
    if(cache_) {
        cache_->put(operand1, operation, operand2, result);
    }

    if(database_) {
        database_->saveCalculation(operand1, operation, operand2, result);
    }

    return result;
}

int CalculatorEngine::calculateDirectly(int operand1, const std::string& operation, int operand2) const {
    // const std::string& oper = request.getOperation();
    if(operation == "+" || operation == "add") {
        return math_utils::add(operand1, operand2);
    }
    if(operation == "-" || operation == "sub") {
        return math_utils::subtract(operand1, operand2);
    }
    if(operation == "*" || operation == "mul") {
        return math_utils::multiply(operand1, operand2);
    }
    if(operation == "/" || operation == "div") {
        //    const int second = request.getSecondOperand().value_or(0);
        if(operand2 == 0) {
            throw DivisionByZeroException();
        }
        return math_utils::divide(operand1, operand2);
    }
    if(operation == "^" || operation == "pow") {
        return math_utils::power(operand1, operand2);
    }
    // if(oper == "!" || oper == "fact") {

    throw InvalidOperationException(operation);
}

int CalculatorEngine::calculateFactorialDirectly(int operand1) const {
    if(operand1 < 0) {
        throw NegativeFactorialException();
    }
    return math_utils::factorial(operand1);
}

// ========== Вспомогательные публичные методы ==========
CalculatorEngine::Stats CalculatorEngine::getStats() const {
    Stats stats;
    stats.cacheHits = cacheHits_;
    stats.dbHits = dbHits_;
    stats.calculations = calculations_;
    stats.cacheSize = cache_ ? cache_->size() : 0;
    return stats;
}

void CalculatorEngine::clearCache() {
    if(cache_) {
        cache_->clear();
    }
    cacheHits_ = dbHits_ = calculations_ = 0;
}

void CalculatorEngine::warmCache() {
    if(cache_ && database_) {
        cache_->warmFromDatabase(*database_);
    }
}

bool CalculatorEngine::hasDatabase() const { return database_ != nullptr; }

bool CalculatorEngine::hasCache() const { return cache_ != nullptr; }