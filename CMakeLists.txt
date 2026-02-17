cmake_minimum_required(VERSION 3.20)
project(Calculator VERSION 1.3.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Building calculator service with gRPC")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Опция для включения ThreadSanitizer (по умолчанию выключена)
option(ENABLE_THREAD_SANITIZER "Enable ThreadSanitizer for debugging" OFF)

if(ENABLE_THREAD_SANITIZER)
    message(STATUS "ThreadSanitizer is ENABLED. Performance will be significantly slower.")
    # Для GCC/Clang добавляем флаг санитайзера
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIE -pie -g -O1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -fPIE -pie -g -O1")
    # Важно добавить флаг и для линковки
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
    # Флаг для получения понятных имен в stacktrace
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
endif()

#поиск PostgesSQL
find_package(PostgreSQL REQUIRED)

if(NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL not found!")
else()
    message(STATUS "PostgreSQL found:")
    message(STATUS "  Include dirs: ${PostgreSQL_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${PostgreSQL_LIBRARIES}")
endif()

# Поиск gRPC и protobuf
find_package(gRPC 1.51 CONFIG REQUIRED)
find_package(Protobuf REQUIRED)

if(NOT gRPC_FOUND)
    message(FATAL_ERROR "gRPC not found! Install: sudo apt-get install libgrpc++-dev protobuf-compiler-grpc")
endif()

if(NOT Protobuf_FOUND)
    message(FATAL_ERROR "Protobuf not found! Install: sudo apt-get install libprotobuf-dev protobuf-compiler")
endif()

message(STATUS "gRPC found: ${gRPC_VERSION}")
message(STATUS "Protobuf found: ${Protobuf_VERSION}")

# ========== Генерация gRPC кода ==========
# Создаем директорию для сгенерированных файлов
set(PROTO_GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

# Путь к proto файлу
set(PROTO_FILE "${CMAKE_SOURCE_DIR}/proto/calculator.proto")

# Получаем имя файла без пути
get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)

# Указываем пути для выходных файлов
set(PROTO_HDR "${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h")
set(PROTO_SRC "${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc")
set(GRPC_HDR "${PROTO_GEN_DIR}/${PROTO_NAME}.grpc.pb.h")
set(GRPC_SRC "${PROTO_GEN_DIR}/${PROTO_NAME}.grpc.pb.cc")

# Явно указываем путь к плагину
set(GRPC_CPP_PLUGIN "/usr/bin/grpc_cpp_plugin")

# Создаем пользовательскую цель для генерации protobuf
add_custom_command(
    OUTPUT ${PROTO_SRC} ${PROTO_HDR}
    COMMAND protoc
        --proto_path=${CMAKE_SOURCE_DIR}/proto
        --cpp_out=${PROTO_GEN_DIR}
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating C++ protobuf files"
)

# Создаем пользовательскую цель для генерации gRPC
add_custom_command(
    OUTPUT ${GRPC_SRC} ${GRPC_HDR}
    COMMAND protoc
        --proto_path=${CMAKE_SOURCE_DIR}/proto
        --grpc_out=${PROTO_GEN_DIR}
        --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating C++ gRPC files"
)

# Создаем список сгенерированных файлов
set(PROTO_GENERATED_FILES
    ${PROTO_SRC}
    ${GRPC_SRC}
)

# Добавляем директорию с сгенерированными файлами в include path
include_directories(${PROTO_GEN_DIR})

# ========== Зависимости FetchContent ==========
include(FetchContent)

# 0. My lib
FetchContent_Declare(
  math_lib
  GIT_REPOSITORY https://github.com/Anton-Pavlenko-Git/math-library
  GIT_TAG main
  GIT_SHALLOW TRUE
)

# 1. nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
  GIT_SHALLOW TRUE
)

# 2. spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.16.0
  GIT_SHALLOW TRUE
)

# 3. GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(math_lib nlohmann_json spdlog googletest)

# ========== Основной сервер ==========
add_executable(calculator
    src/main.cpp
    src/ApplicationRunner.cpp
    src/Daemon.cpp
    src/CalculationRequest.cpp
    src/CalculatorEngine.cpp
    src/Logger.cpp
    src/CalculatorApp.cpp
    src/CalculatorExceptions.cpp
    src/CalculatorServiceImpl.cpp
    src/GrpcServer.cpp
    ${PROTO_GENERATED_FILES}
)

# Добавляем зависимость, чтобы файлы генерировались до компиляции
add_dependencies(calculator calculator_proto_gen)

# Создаем пустую цель для генерации
add_custom_target(calculator_proto_gen DEPENDS ${PROTO_GENERATED_FILES})


set_target_properties(calculator PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME calculator-service
)

add_compile_options(-Wall -Wpedantic -Werror)

target_include_directories(calculator PRIVATE
    include
    src
    ${PROTO_GEN_DIR}               # Для сгенерированных заголовков
    ${PostgreSQL_INCLUDE_DIRS}
    ${math_lib_SOURCE_DIR}/include
)

target_link_libraries(calculator PRIVATE
    gRPC::grpc++                   # gRPC C++ библиотека
    gRPC::grpc++_reflection        # Для рефлексии
    protobuf::libprotobuf          # Protobuf библиотека
    ${PostgreSQL_LIBRARIES}
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    math_utils
    Threads::Threads               # Для многопоточности
)

# ========== Тестовый клиент ==========
add_executable(calculator_client
    tests/calculator_client_test.cpp
    src/Logger.cpp
    ${PROTO_GENERATED_FILES}
)

set_target_properties(calculator_client PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Убираем -Werror для тестового клиента
target_compile_options(calculator_client PRIVATE -Wno-unused-command-line-argument)

target_include_directories(calculator_client PRIVATE
    include
    src
    ${PROTO_GEN_DIR}
)

target_link_libraries(calculator_client PRIVATE
    gRPC::grpc++
    protobuf::libprotobuf
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    Threads::Threads
)

# ========== Тестирование ==========
enable_testing()

if(TARGET GTest::gtest_main)
    message(STATUS "Adding CalculatorApp tests...")
    
       add_executable(test_calculator_app
        tests/test_calculator_app.cpp
        src/Daemon.cpp
        src/CalculationRequest.cpp
        src/CalculatorEngine.cpp
        src/Logger.cpp
        src/CalculatorApp.cpp
        src/CalculatorExceptions.cpp
        src/CalculatorServiceImpl.cpp
        ${PROTO_GENERATED_FILES}
    )
    
    set_target_properties(test_calculator_app PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    target_include_directories(test_calculator_app PRIVATE
        include
        src
        ${PROTO_GEN_DIR}
        ${PostgreSQL_INCLUDE_DIRS}
        ${math_lib_SOURCE_DIR}/include
    )
    
    target_link_libraries(test_calculator_app PRIVATE
        GTest::gtest_main
        gRPC::grpc++
        protobuf::libprotobuf
        ${PostgreSQL_LIBRARIES}
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        math_utils
        Threads::Threads
    )
    
    add_test(NAME CalculatorAppTest COMMAND test_calculator_app)
endif()

# ========== clang-tidy ==========
option(USE_CLANG_TIDY "Enable clang-tidy checks" ON)

if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE 
        NAMES clang-tidy-21 clang-tidy
        DOC "Path to clang-tidy executable"
    )
    
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        
        # Правильная настройка clang-tidy
        set(CMAKE_CXX_CLANG_TIDY 
            ${CLANG_TIDY_EXE}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            -extra-arg=-std=c++20
        )
        
        # Также можно добавить отдельную цель для проверки
        add_custom_target(run-clang-tidy
            COMMAND ${CLANG_TIDY_EXE}
                    --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
                    -p ${CMAKE_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src/*.cpp
                    ${CMAKE_SOURCE_DIR}/include/*.h
            COMMENT "Running clang-tidy on all source files"
            VERBATIM
        )
        
    else()
        message(WARNING "clang-tidy requested but executable not found")
    endif()
endif()

# ========== Установка systemd сервиса ==========
if(UNIX AND NOT APPLE)
    # Файл сервиса systemd
    configure_file(
        ${CMAKE_SOURCE_DIR}/systemd/calculator.service.in
        ${CMAKE_BINARY_DIR}/calculator.service
        @ONLY
    )
    
    # Установка сервиса (опционально)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/calculator.service
            DESTINATION /lib/systemd/system
            COMPONENT systemd)
    
    install(TARGETS calculator
            RUNTIME DESTINATION /usr/local/bin
            COMPONENT application)
endif()
