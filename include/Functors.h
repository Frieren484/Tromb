#pragma once // от двойного включения
#include "Resource.h" // для базовых полей
#include "File.h" // чтоб понимать что такое файл
#include <memory> // для умных указателей

struct SortByName { // функтор (структура-функция) для сортировки по имени
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // перегрузка скобок
        return a->getName() < b->getName(); // просто сравниваем по алфавиту (стринг умеет в оператор <)
    } // конец метода
}; // конец

struct SortBySize { // функтор для сортировки по размеру
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // перегрузка скобок
        return a->calculateSize() < b->calculateSize(); // у кого вес меньше - тот первее
    } // конец
}; // конец

struct SortByDate { // функтор для сортировки по дате создания
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // перегружаем вызов
        return a->getCreationDate().value < b->getCreationDate().value; // чьи секунды меньше (старше) - тот первый
    } // конец
}; // конец
