#pragma once // защита от повторного включения
#include <stdexcept> // библа для ошибок
#include <string> // библа для строк (текста)

class FileSystemException : public std::runtime_error { // кастомная ошибка для нашей ФС
public: // доступно всем
    explicit FileSystemException(const std::string& message); // конструктор с текстом ошибки
}; // закрываем класс

class SecurityException : public std::runtime_error { // ошибка когда нет прав (ACL)
public: // доступно всем
    explicit SecurityException(const std::string& message); // тоже конструктор
}; // закрываем класс
