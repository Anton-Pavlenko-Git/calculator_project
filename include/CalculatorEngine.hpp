#pragma once

#include <string>

#include "CalculationRequest.hpp"
#include "Logger.hpp"
#include "cache/CalculationCache.hpp"
#include "database/Database.hpp"

class CalculationRequest;
class CalculationCache;
class Database;

class CalculatorEngine {
   private:
    // Внутренние зависимости
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<CalculationCache> cache_;
    std::unique_ptr<Database> database_;

    // Статистика
    mutable size_t cacheHits_ = 0;
    mutable size_t dbHits_ = 0;
    mutable size_t calculations_ = 0;

    // Приватные вспомогательные методы
    int calculateDirectly(int operand1, const std::string& operation, int operand2) const;
    int calculateFactorialDirectly(int operand1) const;

    // Основная логика с кешированием (приватная)
    int calculateWithCache(int operand1, const std::string& operation, std::optional<int> operand2) const;

   public:
    int calculate(const ::CalculationRequest& request) const;

    // Конструктор с логгером (без БД)
    explicit CalculatorEngine(std::shared_ptr<Logger> logger = nullptr);

    // Конструктор с БД
    CalculatorEngine(std::shared_ptr<Logger> logger, const std::string& connectionString);

    ~CalculatorEngine() noexcept = default;

    CalculatorEngine(const CalculatorEngine&) = delete;
    CalculatorEngine& operator=(const CalculatorEngine&) = delete;

    CalculatorEngine(CalculatorEngine&&) noexcept;
    CalculatorEngine& operator=(CalculatorEngine&&) noexcept;

    // Статистика
    struct Stats {
        size_t cacheHits;
        size_t dbHits;
        size_t calculations;
        size_t cacheSize;

        double cacheHitRate() const {
            return calculations > 0 ? static_cast<double>(cacheHits) / calculations * 100.0 : 0.0;
        }

        double dbHitRate() const { return calculations > 0 ? static_cast<double>(dbHits) / calculations * 100.0 : 0.0; }
    };

    Stats getStats() const;

    // Утилиты
    void clearCache();
    void warmCache();
    bool hasDatabase() const;
    bool hasCache() const;
};
