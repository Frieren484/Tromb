#pragma once // защита хедера
#include <string> // для стрингов
#include <ctime> // для работы со временем
#include <memory> // для умных указателей (чтоб без утечек памяти)
#include <iomanip> // для манипуляторов вывода (как красиво печатать дату)
#include "Utils.h" // подключаем наши утилиты

struct Date { // структурка для хранения даты создания
    std::time_t value; // само время (секунды с 1970 года)
    
    Date() : value(std::time(nullptr)) {} // конструктор по умолчанию (ставит текущее время)
    Date(std::time_t t) : value(t) {} // конструктор от готового времени

    std::string toString() const { // функция перевода времени в текст
        std::tm tmp = {}; // пустая структурка для локального времени
        localtime_s(&tmp, &value); // парсим секунды в нормальную дату (безопасный вызов)
        char buf[20]; // буфер для текста
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmp); // форматируем дату как надо
        return std::string(buf); // возвращаем готовую строку
    } // конец функции
}; // конец структуры

class Resource { // абстрактный базовый класс для файлов и папок
private: // приватные поля (инкапсуляция)
    std::string name; // имя ресурса
    Date creationDate; // дата создания
    Resource* parent; // указатель на родительскую папку (сырой, чтоб избежать циклических ссылок)

public: // публичные методы
    Resource(const std::string& name); // конструкторчик от имени
    virtual ~Resource() = default; // виртуальный деструктор (чтоб дети удалялись нормально)

    const std::string& getName() const; // геттер имени
    void setName(const std::string& newName); // сеттер имени (с проверкой)

    const Date& getCreationDate() const; // геттер даты
    
    Resource* getParent() const; // геттер бати (родителя)
    void setParent(Resource* newParent); // сеттер бати

    virtual std::unique_ptr<Resource> clone() const = 0; // чистая виртуалка для глубокого копирования
    virtual size_t calculateSize() const = 0; // чистая виртуалка для расчета веса
    virtual void print(int depth = 0) const = 0; // чистая виртуалка для вывода дерева на экран
}; // конец класса
