#pragma once // защита от инклудов
#include "Directory.h" // для работы с папками
#include "File.h" // для файлов
#include <string> // для стрингов
#include <vector> // для векторов

struct SearchCriteria { // структурка для поиска (мультикритериального)
    std::string nameMask; // маска имени (кусочек названия)
    std::string extension; // расширение (которое ищем)
    std::time_t startDate; // искать от этой даты
    std::time_t endDate; // до этой даты

    SearchCriteria() : startDate(0), endDate(-1) {} // конструктор по умолчанию (ставим минус 1 чтобы отключить лимит)
}; // конец структуры

class SearchAnalytics { // класс-помощник для аналитики и поисков
public: // публы
    static std::vector<Resource*> findResources(Directory* root, const SearchCriteria& criteria); // ищем ресурсы по дереву
    static void globalAudit(Directory* root); // вывести статистику (сколько папок, файлов, какой вес)
    static void exportToCSV(Directory* root, const std::string& filename); // экспорт таблички в CSV (как эксель)
}; // конец класса
