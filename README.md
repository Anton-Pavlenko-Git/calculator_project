# Консольная утилита калькулятор для Linux
Вычисляет базовые математические операции: сложение, вычитание, умножение, деление, возведение в степень, факториал.
# Calculator - OOP Refactoring

Modern C++20 calculator with JSON input, logging, and unit tests.

## Features
- 🏗️ **OOP Architecture**: 5 classes with clear responsibilities
- 📦 **JSON Input**: Uses nlohmann/json library
- 📝 **Logging**: Integrated spdlog with debug/info/error levels
- 🚨 **Exception Handling**: Custom exception hierarchy
- ✅ **Unit Tests**: GoogleTest with 100% passing tests
- 🔧 **Modern C++20**: std::optional, smart pointers, etc.
- 🛠️ **VSCode Support**: Preconfigured for remote debugging

## Building
```bash
cmake -B build -S .
cmake --build build
```
