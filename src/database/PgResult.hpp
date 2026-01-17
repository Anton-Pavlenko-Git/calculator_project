#pragma once

#include <libpq-fe.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

/*
 * RAII обертка для результата PostgreSQL запроса (PGresult)
 *
 * Автоматически управляет памятью результата запроса:
 * - Освобождает память в деструкторе
 * - Предоставляет безопасный интерфейс для работы с результатами
 */
class PgResult {
   private:
    PGresult* result_ = nullptr;

   public:
    /*
     * Создает обертку для PGresult
     * result Результат запроса (может быть nullptr)
     *
     * Принимает владение результатом
     */
    explicit PgResult(PGresult* result = nullptr) noexcept : result_(result) {}

    ~PgResult() noexcept { clear(); }

    // Запрещаем копирование
    PgResult(const PgResult&) = delete;
    PgResult& operator=(const PgResult&) = delete;

    // Конструктор перемещения
    PgResult(PgResult&& other) noexcept : result_(other.result_) { other.result_ = nullptr; }

    // Оператор перемещения
    PgResult& operator=(PgResult&& other) noexcept {
        if(this != &other) {
            clear();
            result_ = other.result_;
            other.result_ = nullptr;
        }
        return *this;
    }

    // Освобождает текущий результат
    void clear() noexcept {
        if(result_) {
            PQclear(result_);
            result_ = nullptr;
        }
    }

    // Проверяет, есть ли результат
    bool isValid() const noexcept { return result_ != nullptr; }

    // Неявное преобразование в bool
    explicit operator bool() const noexcept { return isValid(); }

    // Возвращает сырой указатель на PGresult
    PGresult* get() const noexcept { return result_; }

    // Проверяет успешность выполнения запроса
    bool isOk() const noexcept { return result_ && PQresultStatus(result_) == PGRES_COMMAND_OK; }

    // Проверяет, есть ли данные (SELECT запрос)
    bool hasData() const noexcept { return result_ && PQresultStatus(result_) == PGRES_TUPLES_OK; }

    // Возвращает сообщение об ошибке
    std::string errorMessage() const noexcept { return result_ ? PQresultErrorMessage(result_) : "No result"; }

    // Возвращает количество строк в результате
    int rowCount() const noexcept { return result_ ? PQntuples(result_) : 0; }

    // Возвращает количество колонок в результате
    int columnCount() const noexcept { return result_ ? PQnfields(result_) : 0; }

    // Возвращает название колонки по индексу
    std::string columnName(int col) const {
        if(!result_ || col < 0 || col >= columnCount()) {
            throw std::out_of_range("Invalid column index");
        }
        const char* name = PQfname(result_, col);
        return name != nullptr ? name : "";
    }

    // Возвращает значение из результата как строку
    std::string getString(int row, int col) const {
        if(!hasData() || row < 0 || row >= rowCount() || col < 0 || col >= columnCount()) {
            throw std::out_of_range("Invalid row or column index");
        }
        const char* value = PQgetvalue(result_, row, col);
        return value != nullptr ? value : "";
    }

    // Возвращает значение как целое число
    std::optional<int> getInt(int row, int col) const {
        try {
            std::string str = getString(row, col);
            if(str.empty() && PQgetisnull(result_, row, col)) {
                return std::nullopt;  // NULL в БД
            }
            return std::stoi(str);
        } catch(...) {
            return std::nullopt;
        }
    }

    // Проверяет, является ли значение NULL
    bool isNull(int row, int col) const {
        if(!result_ || row < 0 || row >= rowCount() || col < 0 || col >= columnCount()) {
            throw std::out_of_range("Invalid row or column index");
        }
        return PQgetisnull(result_, row, col);
    }

    // Получает первую строку как вектор значений
    std::vector<std::string> getFirstRow() const {
        std::vector<std::string> row;
        if(hasData() && rowCount() > 0) {
            int cols = columnCount();
            for(int i = 0; i < cols; i++) {
                row.push_back(getString(0, i));
            }
        }
        return row;
    }

    // Проверка, что запрос вернул ровно одну строку с одним значением
    bool isSingleValue() const { return hasData() && rowCount() == 1 && columnCount() == 1; }
};