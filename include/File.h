#pragma once // чтоб два раза не подключался
#include "Resource.h" // тянем батю (базовый класс)
#include <string> // для строк

class File : public Resource { // класс Файла, наследуется от Ресурса
private: // скрытые данные
    std::string extension; // расширение файла (типа txt, exe, mp3)
    size_t size; // размерчик в байтах

public: // публы
    File(const std::string& name, const std::string& ext, size_t initialSize = 0); // конструктор
    ~File() override = default; // деструктор по умолчанию (переопределенный)

    const std::string& getExtension() const; // получить расширение
    void setExtension(const std::string& ext); // поменять расширение

    size_t getSize() const; // получить размер
    void setSize(size_t newSize); // задать размер

    std::unique_ptr<Resource> clone() const override; // функция клонирования объекта в памяти
    size_t calculateSize() const override; // посчитать размер файла
    void print(int depth = 0) const override; // напечатать инфу на экран
}; // конец класса File
