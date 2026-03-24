#include "../include/Resource.h" // тянем наш хедер
#include "../include/Exceptions.h" // тянем эксепшены

Resource::Resource(const std::string& name) : creationDate(), parent(nullptr) { // реализация конструктора, родителя пока нет
    setName(name); // сразу ставим имя через сеттер (там есть проверка)
} // конец конструктора

const std::string& Resource::getName() const { // функция получения имени
    return name; // просто возвращаем приватное поле
} // конец функции

void Resource::setName(const std::string& newName) { // функция установки имени
    Utils::validateName(newName); // проверяем имя регуляркой на мусор
    name = newName; // если ок, перезаписываем
} // конец функции

const Date& Resource::getCreationDate() const { // функция получения даты
    return creationDate; // отдаем дату наружу
} // конец функции

Resource* Resource::getParent() const { // геттер родительской ноды
    return parent; // тупо возвращаем указатель
} // конец функции

void Resource::setParent(Resource* newParent) { // сеттер родительской ноды
    parent = newParent; // перезаписываем батю
} // конец функции
