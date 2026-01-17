#pragma once

#include <libpq-fe.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/*
 * RAII обертка для PostgreSQL соединения (PGconn)
 *
 * Автоматически управляет жизненным циклом соединения:
 * - Создает соединение в конструкторе
 * - Закрывает соединение в деструкторе
 * - Запрещает копирование (только перемещение)
 */
class PgConnection {
   private:
    PGconn* connection_ = nullptr;

    // Вспомогательная функция для закрытия соединения
    void close() noexcept {
        if(connection_) {
            PQfinish(connection_);
            connection_ = nullptr;
        }
    }

   public:
    /*
     * Создает соединение с PostgreSQL
     * connectionString Строка подключения в формате libpq
     * std::runtime_error если не удалось подключиться
     *
     */
    explicit PgConnection(const std::string& connectionString) {
        connection_ = PQconnectdb(connectionString.c_str());

        if(!connection_ || PQstatus(connection_) != CONNECTION_OK) {
            std::string error = connection_ ? PQerrorMessage(connection_) : "Failed to allocate connection";
            close();
            throw std::runtime_error("PostgreSQL connection failed: " + error);
        }

        // Устанавливаем кодировку UTF-8
        PQsetClientEncoding(connection_, "UTF8");
    }

    // Деструктор - автоматически закрывает соединение
    ~PgConnection() noexcept { close(); }

    // Запрещаем копирование
    PgConnection(const PgConnection&) = delete;
    PgConnection& operator=(const PgConnection&) = delete;

    PgConnection(PgConnection&& other) noexcept : connection_(other.connection_) { other.connection_ = nullptr; }

    PgConnection& operator=(PgConnection&& other) noexcept {
        if(this != &other) {
            close();
            connection_ = other.connection_;
            other.connection_ = nullptr;
        }
        return *this;
    }

    // Проверяет, активно ли соединение

    bool isConnected() const noexcept { return connection_ && PQstatus(connection_) == CONNECTION_OK; }

    /*
     * Возвращает сырой указатель на PGconn
     * Использовать только для передачи в функции libpq
     */
    PGconn* get() const noexcept { return connection_; }

    // Неявное преобразование в bool для проверки соединения
    explicit operator bool() const noexcept { return isConnected(); }

    /*
     * Выполняет SQL запрос без параметров
     * sql SQL запрос
     * PGresult* результат запроса (нужно очистить с помощью PQclear)
     * std::runtime_error если соединение не активно
     */
    PGresult* exec(const std::string& sql) const {
        if(!isConnected()) {
            throw std::runtime_error("Database connection is not active");
        }
        return PQexec(connection_, sql.c_str());
    }

    /*
     * Выполняет SQL запрос с параметрами
     * sql SQL запрос с плейсхолдерами $1, $2, ...
     * params Вектор строковых параметров
     * PGresult* результат запроса
     */
    PGresult* execParams(const std::string& sql, const std::vector<const char*>& params) const {
        if(!isConnected()) {
            throw std::runtime_error("Database connection is not active");
        }
        return PQexecParams(
            connection_, sql.c_str(), static_cast<int>(params.size()),
            nullptr,  // типы параметров (auto)
            params.data(),
            nullptr,  // длины параметров (text = null)
            nullptr,  // форматы параметров (text = 0)
            0
        );  // формат результата (text = 0)
    }

    // Начинает транзакцию
    void beginTransaction() const noexcept {
        PGresult* res = exec("BEGIN");
        PQclear(res);
    }

    // Фиксирует транзакцию
    void commit() const noexcept {
        PGresult* res = exec("COMMIT");
        PQclear(res);
    }

    // Откатывает транзакцию
    void rollback() const noexcept {
        PGresult* res = exec("ROLLBACK");
        PQclear(res);
    }

    // Возвращает сообщение об ошибке
    std::string errorMessage() const noexcept { return connection_ ? PQerrorMessage(connection_) : "No connection"; }

    // Возвращает версию сервера PostgreSQL
    int serverVersion() const noexcept { return connection_ ? PQserverVersion(connection_) : 0; }

    // Проверяет, находится ли соединение в режиме транзакции
    bool isInTransaction() const noexcept { return connection_ && PQtransactionStatus(connection_) == PQTRANS_INTRANS; }
};