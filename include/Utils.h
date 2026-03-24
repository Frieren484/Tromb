#pragma once // защита от двойного инклуда
#include <string> // для стрингов
#include <regex> // для регулярок (проверка текста)
#include "Exceptions.h" // тянем наши ошибки

class Utils { // чисто класс-помощник
public: // публы
    static bool validateName(const std::string& name); // проверка имени на плохие символы
    static std::string sanitizeCSV(const std::string& input); // чистка для экспорта в эксель
}; // конец класса
