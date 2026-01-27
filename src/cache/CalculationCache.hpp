#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "Logger.hpp"

/*
 * Кеш вычислений в памяти
 *
 * Хранит результаты вычислений для быстрого доступа.
 * Автоматически обновляется при сохранении в БД.
 */
class CalculationCache {
   private:
    std::unordered_map<std::string, int> cache_;
    std::shared_ptr<Logger> logger_;

    // Генерация строкового ключа
    static std::string makeKey(int operand1, const std::string& operation, std::optional<int> operand2) {
        std::string key = std::to_string(operand1) + operation;
        if(operand2.has_value()) {
            key += std::to_string(operand2.value());
        }
        return key;
    }

   public:
    explicit CalculationCache(std::shared_ptr<Logger> logger = nullptr) : logger_(std::move(logger)) {}

    // Добавляет результат в кеш
    void put(int operand1, const std::string& operation, std::optional<int> operand2, int result) {
        std::string key = makeKey(operand1, operation, operand2);
        cache_[key] = result;

        if(logger_) {
            logger_->debug("Cache put: " + key + " = " + std::to_string(result));
        }
    }

    /*
     * Ищет результат в кеше
     * optional с результатом если найден, иначе nullopt
     */
    std::optional<int> get(int operand1, const std::string& operation, std::optional<int> operand2) const {
        std::string key = makeKey(operand1, operation, operand2);
        auto it = cache_.find(key);

        if(it != cache_.end()) {
            if(logger_) {
                logger_->debug("Cache hit: " + key + " = " + std::to_string(it->second));
            }
            return it->second;
        }
        if(logger_) {
            logger_->debug("Cache miss: " + key);
        }
        return std::nullopt;
    }

    // Проверяет наличие результата в кеше
    bool contains(int operand1, const std::string& operation, std::optional<int> operand2) const {
        std::string key = makeKey(operand1, operation, operand2);
        return cache_.find(key) != cache_.end();
    }

    // Удаляет результат из кеша
    void remove(int operand1, const std::string& operation, std::optional<int> operand2) {
        std::string key = makeKey(operand1, operation, operand2);
        cache_.erase(key);

        if(logger_) {
            logger_->debug("Cache remove: " + key);
        }
    }

    // Очищает весь кеш
    void clear() {
        cache_.clear();
        if(logger_) {
            logger_->info("Cache cleared");
        }
    }

    // Возвращает количество элементов в кеше
    std::size_t size() const { return cache_.size(); }

    /*
     * Прогревает кеш из базы данных
     * DatabaseType Тип класса базы данных (должен иметь метод loadAllCalculations())
     */
    template <typename DatabaseType>
    void warmFromDatabase(DatabaseType& db) {
        auto calculations = db.loadAllCalculations();

        for(const auto& number : calculations) {
            put(number.first, number.operation, (number.second != 0) ? std::optional<int>(number.second) : std::nullopt,
                number.result);
        }

        if(logger_) {
            logger_->info(
                "Cache warmed from database: loaded " + std::to_string(calculations.size()) + " calculations"
            );
        }
    }

    // Возвращает статистику кеша
    struct CacheStats {
        std::size_t size;
        std::size_t memory_estimate;  // примерная оценка памяти в байтах
    };

    CacheStats getStats() const {
        // Более точная оценка памяти: ключ (строка)+ значение + накладные расходы unordered_map
        std::size_t memory = 0;
        for(const auto& [key, value] : cache_) {
            memory += key.capacity() * sizeof(char) + sizeof(int) + 32;  // Накладные расходы
        }
        return CacheStats{cache_.size(), memory};
    }
};