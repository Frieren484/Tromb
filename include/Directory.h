#pragma once // защита от повторного включения (чтоб компилятор не ругался)
#include "Resource.h" // подключаем батю
#include <vector> // подключаем динамические массивы
#include <memory> // для умных указателей (чтоб не было memory leaks)

enum class AccessLevel { GUEST = 0, USER = 1, ADMIN = 2 }; // уровни доступа: гость, юзер, админ

class Directory : public Resource { // папка тоже ресурс, наследуемся
private: // скрытые данные
    std::vector<std::unique_ptr<Resource>> children; // вектор умных указателей на детей (список того, что лежит в папке)
    AccessLevel accessLevel; // требуемые права для работы с папкой

public: // доступно снаружи
    Directory(const std::string& name, AccessLevel level = AccessLevel::USER); // конструктор с именем и правами (по дефолту юзер)
    ~Directory() override = default; // деструктор по умолчанию (вектор сам всё почистит, кайф)

    AccessLevel getAccessLevel() const; // получить уровень доступа
    void setAccessLevel(AccessLevel level); // поменять уровень доступа

    void addResource(std::unique_ptr<Resource> child); // положить файл/папку внутрь
    bool removeResource(const std::string& childName, AccessLevel userLevel); // удалить файл/папку (с проверкой прав)
    
    std::unique_ptr<Resource> releaseResource(const std::string& childName, AccessLevel userLevel); // достать файл/папку из вектора для перемещения

    const std::vector<std::unique_ptr<Resource>>& getChildren() const; // получить список детей (для поиска/сортировки)

    std::unique_ptr<Resource> clone() const override; // глубокая копия всей папки со всеми потрохами
    size_t calculateSize() const override; // посчитать размер папки (сумма размеров всех детей)
    void print(int depth = 0) const override; // вывести дерево папки на экран
}; // конец класса папки
