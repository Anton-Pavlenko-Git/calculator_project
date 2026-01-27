#pragma once

#include <optional>
#include <vector>

#include "Number.hpp"
#include "PgConnection.hpp"
#include "PgResult.hpp"

/*
 * Класс для работы с базой данных calculator
 *
 * Предоставляет высокоуровневый интерфейс для:
 * - Сохранения результатов вычислений
 * - Поиска результатов в кеше (БД)
 * - Загрузки истории для прогрева кеша
 */
class Database {
   private:
    PgConnection connection_;

    // Подготовленные выражения (для производительности)
    bool prepared_ = false;

    void prepareStatements() {
        if(!prepared_) {
            // Здесь можно было бы использовать PQprepare,
            // но для простоты используем обычные запросы
            prepared_ = true;
        }
    }

   public:
    /*
     *  Создает соединение с БД
     *  connectionString Строка подключения к PostgreSQL
     */
    explicit Database(const std::string& connectionString) : connection_(connectionString) {
        if(!connection_.isConnected()) {
            throw std::runtime_error("Failed to connect to database");
        }

        PgResult noticeResult(connection_.exec("SET client_min_messages TO WARNING;"));  // Отключаем NOTICE сообщения

        prepareStatements();
        createTablesIfNotExist();
    }

    // Проверяет подключение к БД
    bool isConnected() const { return connection_.isConnected(); }

    // Создает таблицы если их нет
    void createTablesIfNotExist() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS calculation_history (
                id SERIAL PRIMARY KEY,
                operand1 INTEGER NOT NULL,
                operation VARCHAR(10) NOT NULL,
                operand2 INTEGER,
                result INTEGER NOT NULL,
                calculated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                
                CONSTRAINT unique_calculation UNIQUE (operand1, operation, operand2)
            );
            
            CREATE INDEX IF NOT EXISTS idx_calculation_lookup 
            ON calculation_history (operand1, operation, operand2);
            
            CREATE INDEX IF NOT EXISTS idx_factorial 
            ON calculation_history (operand1, operation) 
            WHERE operand2 IS NULL;
        )";

        PgResult result(connection_.exec(sql));
        if(!result.isOk()) {
            throw std::runtime_error("Failed to create tables: " + result.errorMessage());
        }
    }

    /*
     * Сохраняет результат вычисления в БД
     * operand1 Первый операнд
     * operation Операция (+, -, *, /, ^, !)
     * operand2 Второй операнд (nullopt для факториала)
     * result Результат вычисления
     * true если успешно сохранено
     */
    bool saveCalculation(int operand1, const std::string& operation, std::optional<int> operand2, int result) {
        std::string sql;
        std::vector<const char*> params;

        if(operand2.has_value()) {
            // Бинарная операция
            sql =
                "INSERT INTO calculation_history (operand1, operation, operand2, result) "
                "VALUES ($1, $2, $3, $4) "
                "ON CONFLICT (operand1, operation, operand2) "
                "DO UPDATE SET result = EXCLUDED.result, calculated_at = CURRENT_TIMESTAMP";

            // Преобразуем параметры в const char*
            std::string op1_str = std::to_string(operand1);
            std::string op2_str = std::to_string(*operand2);
            std::string res_str = std::to_string(result);

            params = {op1_str.c_str(), operation.c_str(), op2_str.c_str(), res_str.c_str()};
        } else {
            // Унарная операция (факториал)
            sql =
                "INSERT INTO calculation_history (operand1, operation, operand2, result) "
                "VALUES ($1, $2, NULL, $3) "
                "ON CONFLICT (operand1, operation, operand2) "
                "DO UPDATE SET result = EXCLUDED.result, calculated_at = CURRENT_TIMESTAMP";

            std::string op1_str = std::to_string(operand1);
            std::string res_str = std::to_string(result);

            params = {op1_str.c_str(), operation.c_str(), res_str.c_str()};
        }

        PgResult dbResult(connection_.execParams(sql, params));
        return dbResult.isOk();
    }

    /*
     * Ищет результат вычисления в БД
     * operand1 Первый операнд
     * operation Операция
     * operand2 Второй операнд (nullopt для факториала)
     * optional с результатом если найден, иначе nullopt
     */
    std::optional<int> findCalculation(int operand1, const std::string& operation, std::optional<int> operand2) {
        std::string sql;
        std::vector<const char*> params;

        if(operand2.has_value()) {
            // Бинарная операция
            sql =
                "SELECT result FROM calculation_history "
                "WHERE operand1 = $1 AND operation = $2 AND operand2 = $3";

            std::string op1_str = std::to_string(operand1);
            std::string op2_str = std::to_string(*operand2);

            params = {op1_str.c_str(), operation.c_str(), op2_str.c_str()};
        } else {
            // Унарная операция (факториал)
            sql =
                "SELECT result FROM calculation_history "
                "WHERE operand1 = $1 AND operation = $2 AND operand2 IS NULL";

            std::string op1_str = std::to_string(operand1);

            params = {op1_str.c_str(), operation.c_str()};
        }

        PgResult dbResult(connection_.execParams(sql, params));

        if(dbResult.hasData() && dbResult.rowCount() > 0) {
            return dbResult.getInt(0, 0);
        }

        return std::nullopt;
    }

    // Загружает всю историю вычислений для прогрева кеша
    // Вектор структур Number
    std::vector<Number> loadAllCalculations() {
        std::vector<Number> history;

        const char* sql =
            "SELECT operand1, operation, operand2, result "
            "FROM calculation_history "
            "ORDER BY calculated_at DESC";

        PgResult result(connection_.exec(sql));

        if(!result.hasData()) {
            return history;
        }

        for(int i = 0; i < result.rowCount(); i++) {
            auto op1 = result.getInt(i, 0);
            auto operation = result.getString(i, 1);
            auto op2 = result.getInt(i, 2);  // может быть nullopt
            auto res = result.getInt(i, 3);

            if(op1 && res) {
                if(op2.has_value()) {
                    history.emplace_back(*op1, *op2, operation, *res);
                } else {
                    history.emplace_back(*op1, operation, *res);
                }
            }
        }

        return history;
    }

    // Возвращает количество записей в истории
    int getHistoryCount() {
        PgResult result(connection_.exec("SELECT COUNT(*) FROM calculation_history"));

        if(result.hasData() && result.rowCount() > 0) {
            auto count = result.getInt(0, 0);
            return count.value_or(0);
        }

        return 0;
    }

    // Очищает историю вычислений
    // true если успешно
    bool clearHistory() {
        PgResult result(connection_.exec("DELETE FROM calculation_history"));
        return result.isOk();
    }

    // Возвращает статистику по операциям
    std::vector<std::pair<std::string, int>> getOperationStats() {
        std::vector<std::pair<std::string, int>> stats;

        const char* sql =
            "SELECT operation, COUNT(*) as count "
            "FROM calculation_history "
            "GROUP BY operation "
            "ORDER BY count DESC";

        PgResult result(connection_.exec(sql));

        if(!result.hasData()) {
            return stats;
        }

        for(int i = 0; i < result.rowCount(); i++) {
            auto operation = result.getString(i, 0);
            auto count = result.getInt(i, 1);

            if(count) {
                stats.emplace_back(operation, *count);
            }
        }

        return stats;
    }
};