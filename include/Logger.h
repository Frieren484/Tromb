#pragma once // шоб не дублировалось
#include <string> // подключаем строки
#include <iostream> // для вывода в консоль
#include <fstream> // для файлов логов

enum class LogLevel { INFO, WARNING, ERR }; // уровни логов: инфо, варн, ошибка

class Logger { // класс логгера
public: // публичка
    static void log(LogLevel level, const std::string& message); // функция записи лога (статика чтоб не создавать объект)
}; // конец класса
