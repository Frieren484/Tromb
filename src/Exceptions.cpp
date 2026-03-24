#include "../include/Exceptions.h" // тянем наш заголовочный файл

FileSystemException::FileSystemException(const std::string& message) // реализация конструкора
    : std::runtime_error(message) {} // передаем текст в стандартную ошибку

SecurityException::SecurityException(const std::string& message) // реализация конструктора безопасности
    : std::runtime_error(message) {} // тоже прокидываем текст ошибки дальше
