#pragma once

#include <atomic>
#include <csignal>
#include <functional>
// #include <memory>
#include <thread>
#include <unordered_map>

#include "Logger.hpp"

/*
 * Идиоматический C++ демон с явным запуском
 * Полное соблюдение правила пяти, корректная обработка сигналов
 */
class Daemon {
   public:
    using SignalCallback = std::function<void()>;

    /*
     * Конструктор демона
     * logger Общий логгер для логирования
     * std::invalid_argument если logger == nullptr
     */
    explicit Daemon(std::shared_ptr<Logger> logger);

    // Деструктор - гарантирует корректную остановку
    ~Daemon();

    // Правило пяти
    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;
    Daemon(Daemon&& other) noexcept;
    Daemon& operator=(Daemon&& other) noexcept;

    /*
     * Явный запуск демона и обработки сигналов
     * std::runtime_error если демон уже запущен
     */
    void start();

    /*
     * Остановка демона
     * Гарантирует noexcept
     */
    void stop() noexcept;

    /*
     * Регистрация пользовательского обработчика сигнала
     * signal Номер сигнала (SIGTERM, SIGINT, SIGHUP, SIGUSR1, SIGUSR2)
     * callback Функция-обработчик
     */
    void registerSignalHandler(int signal, SignalCallback callback);

    /*
     * Проверка, работает ли демон
     * true если демон запущен
     */
    bool isRunning() const noexcept { return running_.load(); }

    /*
     * Преобразование текущего процесса в демона (отсоединение от терминала)
     * После вызова процесс становится фоновым демоном
     */
    static void daemonize();

   private:
    std::shared_ptr<Logger> logger_;
    std::atomic<bool> running_{false};
    std::atomic<bool> shutdown_requested_{false};
    std::unique_ptr<std::thread> signal_thread_;
    std::unordered_map<int, SignalCallback> signal_handlers_;

    // Основной цикл обработки сигналов
    void signalProcessingLoop();

    // Инициализация обработчиков сигналов по умолчанию
    void initializeDefaultHandlers();

    /*
     * Настройка маски сигналов для блокировки в потоке
     * signal_set Набор сигналов для настройки
     */
    void setupSignalMask(sigset_t* signal_set);
};