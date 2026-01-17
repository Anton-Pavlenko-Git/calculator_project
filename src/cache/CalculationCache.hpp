#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>

// Ключ для кеша вычислений
struct CacheKey {
    int operand1;
    std::string operation;
    std::optional<int> operand2;

    // Для использования в unordered_map нужны операторы сравнения и хеш-функция
    bool operator==(const CacheKey& other) const {
        return operand1 == other.operand1 && operation == other.operation && operand2 == other.operand2;
    }
};

// Хеш-функция для CacheKey
struct CacheKeyHash {
    std::size_t operator()(const CacheKey& key) const {
        std::size_t h1 = std::hash<int>{}(key.operand1);
        std::size_t h2 = std::hash<std::string>{}(key.operation);
        std::size_t h3 = key.operand2.has_value() ? std::hash<int>{}(*key.operand2) : std::hash<int>{}(0);

        // Комбинируем хеши (используем boost::hash_combine алгоритм)
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

/*
 * Кеш вычислений в памяти
 *
 * Хранит результаты вычислений для быстрого доступа.
 * Автоматически обновляется при сохранении в БД.
 */
class CalculationCache {
   private:
    std::unordered_map<CacheKey, int, CacheKeyHash> cache_;

   public:
    explicit CalculationCache() = default;

    // Добавляет результат в кеш
    void put(int operand1, const std::string& operation, std::optional<int> operand2, int result) {
        CacheKey key{operand1, operation, operand2};
        cache_[key] = result;
    }

    /*
     * Ищет результат в кеше
     * optional с результатом если найден, иначе nullopt
     */
    std::optional<int> get(int operand1, const std::string& operation, std::optional<int> operand2) const {
        CacheKey key{operand1, operation, operand2};
        auto it = cache_.find(key);

        if(it != cache_.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    // Проверяет наличие результата в кеше
    bool contains(int operand1, const std::string& operation, std::optional<int> operand2) const {
        CacheKey key{operand1, operation, operand2};
        return cache_.find(key) != cache_.end();
    }

    // Удаляет результат из кеша
    void remove(int operand1, const std::string& operation, std::optional<int> operand2) {
        CacheKey key{operand1, operation, operand2};
        cache_.erase(key);
    }

    // Очищает весь кеш
    void clear() { cache_.clear(); }

    // Возвращает количество элементов в кеше
    std::size_t size() const { return cache_.size(); }

    /*
     * Прогревает кеш из базы данных
     * DatabaseType Тип класса базы данных (должен иметь метод loadAllCalculations())
     */
    template <typename DatabaseType>
    void warmFromDatabase(DatabaseType& db) {
        auto calculations = db.loadAllCalculations();

        for(const auto& [operand1, operation, operand2, result] : calculations) {
            put(operand1, operation, operand2, result);
        }

        std::cout << "[Cache] Warmed from database: loaded " << calculations.size() << " calculations" << std::endl;
    }

    // Возвращает статистику кеша
    struct CacheStats {
        std::size_t size;
        std::size_t memory_estimate;  // примерная оценка памяти в байтах
    };

    CacheStats getStats() const {
        // Грубая оценка памяти: ключ + значение + накладные расходы unordered_map
        std::size_t memory = cache_.size() * (sizeof(CacheKey) + sizeof(int) + 32);

        return CacheStats{cache_.size(), memory};
    }
};