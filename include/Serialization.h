#pragma once // защита от инклудов
#include "Directory.h" // папка (корень для сохранки)
#include <string> // стринги
#include <memory> // уникальные указатели

class Serialization { // класс-помощник для сохранений
public: // публы
    static void saveToFile(Directory* root, const std::string& filename); // метод: сохранить дерево в файл
    static std::unique_ptr<Directory> loadFromFile(const std::string& filename); // метод: загрузить дерево из архива
}; // конец класса
