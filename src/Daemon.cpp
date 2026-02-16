#include "Daemon.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <system_error>

Daemon::Daemon(std::shared_ptr<Logger> logger) : logger_(std::move(logger)) {
    if(!logger_) {
        throw std::invalid_argument("Logger cannot be null");
    }

    logger_->debug("Daemon object created");
    initializeDefaultHandlers();
}

Daemon::~Daemon() {
    stop();
    logger_->debug("Daemon object destroyed");
}

Daemon::Daemon(Daemon&& other) noexcept
    : logger_(std::move(other.logger_))
    , running_(other.running_.load())
    , shutdown_requested_(other.shutdown_requested_.load())
    , signal_thread_(std::move(other.signal_thread_))
    , signal_handlers_(std::move(other.signal_handlers_)) {
    // Сбрасываем состояние у перемещаемого объекта
    other.running_.store(false);
    other.shutdown_requested_.store(false);

    logger_->debug("Daemon object moved");
}

Daemon& Daemon::operator=(Daemon&& other) noexcept {
    if(this != &other) {
        // Останавливаем текущий демон
        stop();

        // Перемещаем ресурсы
        logger_ = std::move(other.logger_);
        running_.store(other.running_.load());
        shutdown_requested_.store(other.shutdown_requested_.load());

        // Если текущий поток присоединяем или отсоединяем перед перемещением
        if(signal_thread_.joinable()) {
            signal_thread_.join();
        }

        signal_thread_ = std::move(other.signal_thread_);
        signal_handlers_ = std::move(other.signal_handlers_);

        // Сбрасываем состояние у перемещаемого объекта
        other.running_.store(false);
        other.shutdown_requested_.store(false);

        logger_->debug("Daemon object move-assigned");
    }
    return *this;
}

void Daemon::start() {
    if(running_.exchange(true)) {
        throw std::runtime_error("Daemon is already running");
    }

    logger_->info("Starting daemon");

    // Сбрасываем флаг запроса остановки
    shutdown_requested_.store(false);

    // Запускаем поток обработки сигналов
    signal_thread_ = std::thread(&Daemon::signalProcessingLoop, this);

    logger_->info("Daemon started successfully");
}

void Daemon::stop() noexcept {
    if(!running_.exchange(false)) {
        return;  // Уже остановлен
    }

    logger_->info("Stopping daemon");

    // Запрашиваем остановку
    shutdown_requested_.store(true);

    // Если поток обработки сигналов работает, отправляем SIGUSR1 для пробуждения
    if(signal_thread_.joinable()) {
        // Отправляем сигнал для пробуждения sigwait()
        pthread_kill(signal_thread_.native_handle(), SIGUSR1);

        // Ждем завершения потока
        signal_thread_.join();
    }

    logger_->info("Daemon stopped");
}

void Daemon::registerSignalHandler(int signal, SignalCallback callback) {
    signal_handlers_[signal] = std::move(callback);
    logger_->debug("Registered handler for signal " + std::to_string(signal));
}

void Daemon::initializeDefaultHandlers() {
    // Обработчик SIGTERM
    signal_handlers_[SIGTERM] = [this]() {
        logger_->info("SIGTERM received - graceful shutdown");
        shutdown_requested_.store(true);
    };

    // Обработчик SIGINT
    signal_handlers_[SIGINT] = [this]() {
        logger_->info("SIGINT received - interrupted");
        shutdown_requested_.store(true);
    };

    // Обработчик SIGHUP
    signal_handlers_[SIGHUP] = [this]() {
        logger_->info("SIGHUP received");
        // Пока ничего не делаем
    };

    // Обработчик SIGUSR1 - для пробуждения sigwait() при stop()
    signal_handlers_[SIGUSR1] = []() {
        // Пустой обработчик - просто пробуждает sigwait()
    };
}

void Daemon::setupSignalMask(sigset_t* signal_set) {
    // Очищаем набор сигналов
    sigemptyset(signal_set);

    // Добавляем сигналы, которые хотим обрабатывать
    sigaddset(signal_set, SIGTERM);  // Завершение процесса
    sigaddset(signal_set, SIGINT);   // Прерывание (Ctrl+C)
    sigaddset(signal_set, SIGHUP);   // Перезагрузка конфигурации
    sigaddset(signal_set, SIGUSR1);  // Пользовательский сигнал 1
    sigaddset(signal_set, SIGUSR2);  // Пользовательский сигнал 2

    // Игнорируем SIGPIPE глобально (чтобы не падать при разрыве сетевого соединения)
    std::signal(SIGPIPE, SIG_IGN);
}

void Daemon::signalProcessingLoop() {
    logger_->info("Signal processing thread started");

    sigset_t signal_set;
    setupSignalMask(&signal_set);

    // Блокируем сигналы в текущем потоке
    // Они будут доставляться через sigwait(), а не через асинхронные обработчики
    if(pthread_sigmask(SIG_BLOCK, &signal_set, nullptr) != 0) {
        logger_->error("Failed to set signal mask");
        running_.store(false);
        return;
    }

    int received_signal = 0;

    while(running_.load() && !shutdown_requested_.load()) {
        // Ожидаем сигналов (блокирующий вызов)
        int result = sigwait(&signal_set, &received_signal);

        if(result != 0) {
            logger_->error("sigwait failed with error: " + std::to_string(result));
            continue;
        }

        logger_->debug("Received signal: " + std::to_string(received_signal));

        // Находим и вызываем зарегистрированный обработчик
        auto it = signal_handlers_.find(received_signal);
        if(it != signal_handlers_.end() && it->second) {
            try {
                it->second();  // Вызываем пользовательский обработчик
            } catch(const std::exception& e) {
                logger_->error("Error in signal handler: " + std::string(e.what()));
            }
        } else {
            logger_->debug("No handler registered for signal: " + std::to_string(received_signal));
        }

        // Автоматическая остановка для shutdown сигналов
        if(received_signal == SIGTERM || received_signal == SIGINT) {
            logger_->info("Shutdown signal processed, stopping daemon");
            shutdown_requested_.store(true);
            break;
        }
    }

    // Разблокируем сигналы перед завершением
    sigset_t empty_set;
    sigemptyset(&empty_set);
    pthread_sigmask(SIG_SETMASK, &empty_set, nullptr);

    // Обновляем состояние
    running_.store(false);

    logger_->info("Signal processing thread finished");
}

void Daemon::daemonize() {
    // 1. Первый fork - отсоединение от терминала
    pid_t pid = fork();
    if(pid < 0) {
        throw std::system_error(errno, std::system_category(), "First fork failed");
    }
    if(pid > 0) {
        exit(EXIT_SUCCESS);  // Завершаем родительский процесс
    }

    // 2. Создаем новую сессию и становиться ее лидером
    if(setsid() < 0) {
        throw std::system_error(errno, std::system_category(), "setsid failed");
    }

    // 3. Второй fork - гарантия, что процесс не сможет получить управляющий терминал
    pid = fork();
    if(pid < 0) {
        throw std::system_error(errno, std::system_category(), "Second fork failed");
    }
    if(pid > 0) {
        exit(EXIT_SUCCESS);  // Завершаем первый дочерний процесс
    }

    // 4. Устанавливаем безопасную маску прав на файлы
    umask(0);

    // 5. Меняем рабочую директорию на корневую (или другую безопасную)
    if(chdir("/") < 0) {
        // Не критичная ошибка, продолжаем
    }

    // 6. Закрываем стандартные файловые дескрипторы
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 7. Перенаправляем стандартные потоки в /dev/null
    int devnull = open("/dev/null", O_RDWR);
    if(devnull == -1) {
        throw std::system_error(errno, std::system_category(), "Cannot open /dev/null");
    }

    if(dup2(devnull, STDIN_FILENO) == -1 || dup2(devnull, STDOUT_FILENO) == -1 || dup2(devnull, STDERR_FILENO) == -1) {
        close(devnull);
        throw std::system_error(errno, std::system_category(), "dup2 failed");
    }

    if(devnull > STDERR_FILENO) {
        close(devnull);
    }
}