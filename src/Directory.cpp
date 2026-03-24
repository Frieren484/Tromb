#include "../include/Directory.h" // тянем заголовок папки
#include "../include/Exceptions.h" // тянем наши ошибки
#include "../include/Logger.h" // логгер для записи действий
#include "../include/File.h" // тянем заголовок файла для доступа к расширениям
#include <iostream> // для вывода в консоль
#include <algorithm> // для std::find_if (поиска)

Directory::Directory(const std::string& name, AccessLevel level) // конструктор папки
    : Resource(name), accessLevel(level) {} // вызываем батю и ставим уровень доступа

AccessLevel Directory::getAccessLevel() const { // получить права
    return accessLevel; // возвращаем значение
} // конец метода

void Directory::setAccessLevel(AccessLevel level) { // установить новые права
    accessLevel = level; // перезаписываем
} // ок

void Directory::addResource(std::unique_ptr<Resource> child) { // добавить файл/папку внутрь
    child->setParent(this); // говорим ребенку, что мы теперь его батя
    children.push_back(std::move(child)); // запихиваем его в вектор (через мув, так как это unique_ptr)
    Logger::log(LogLevel::INFO, "Добавлен ресурс в директорию: " + getName()); // пишем в лог
} // конец

bool Directory::removeResource(const std::string& childName, AccessLevel userLevel) { // удалить файл по имени
    if (userLevel < accessLevel) { // если не хватает прав
        Logger::log(LogLevel::ERR, "Отказано в доступе: невозможно удалить " + childName); // пишем ошибку в лог
        throw SecurityException("Отказано в доступе: недостаточно прав для удаления ресурса."); // кидаем эксепшен пользователю
    } // конец ифа

    auto it = std::find_if(children.begin(), children.end(), // ищем элемент в векторе
        [&childName](const std::unique_ptr<Resource>& r) { 
            if (r->getName() == childName) return true; // если имя совпало напрямую (без формата или папка)
            if (auto f = dynamic_cast<File*>(r.get())) { // если это файл
                return (f->getName() + "." + f->getExtension() == childName); // проверяем полное совпадение имя.формат
            }
            return false; // не нашлось
        }); // конец лямбды

    if (it != children.end()) { // если нашли (итератор не дошел до конца)
        children.erase(it); // удаляем его (unique_ptr сам почистит память, ура)
        Logger::log(LogLevel::INFO, "Удален ресурс " + childName); // логируем успех
        return true; // возвращаем тру
    } // конец ифа
    return false; // если не нашли - фолс
} // конец метода

std::unique_ptr<Resource> Directory::releaseResource(const std::string& childName, AccessLevel userLevel) { // достать элемент из папки (для перемещения)
    if (userLevel < accessLevel) { // проверка прав
        Logger::log(LogLevel::ERR, "Отказано в доступе: невозможно переместить " + childName); // в лог
        throw SecurityException("Отказано в доступе: недостаточно прав для перемещения ресурса."); // в лицо
    } // конец ифа

    auto it = std::find_if(children.begin(), children.end(), // ищем
        [&childName](const std::unique_ptr<Resource>& r) { 
            if (r->getName() == childName) return true;
            if (auto f = dynamic_cast<File*>(r.get())) {
                return (f->getName() + "." + f->getExtension() == childName);
            }
            return false;
        }); // по полному имени

    if (it != children.end()) { // если нашли
        std::unique_ptr<Resource> res = std::move(*it); // забираем владение (вытаскиваем из вектора в временную переменную)
        children.erase(it); // стираем пустую дырку в векторе
        res->setParent(nullptr); // обнуляем батю
        return res; // отдаем ресурс наружу
    } // конец ифа
    return nullptr; // если не нашли - отдаем нулл
} // конец

const std::vector<std::unique_ptr<Resource>>& Directory::getChildren() const { // получить всех детей
    return children; // отдаем константную ссылку на вектор
} // конец

std::unique_ptr<Resource> Directory::clone() const { // глубокое копирование (сложная штука)
    auto clonedDir = std::make_unique<Directory>(getName(), accessLevel); // делаем пустую копию этой же папки
    for (const auto& child : children) { // бежим по всем детям
        clonedDir->addResource(child->clone()); // рекурсивно клонируем каждого ребенка и запихиваем в новую папку
    } // конец цикла
    return clonedDir; // отдаем готовую клонированную ветку
} // конец

size_t Directory::calculateSize() const { // посчитать суммарный размер папки
    size_t total = 0; // счетчик с нуля
    for (const auto& child : children) { // бежим по детям
        total += child->calculateSize(); // прибавляем вес каждого ребенка (полиморфизм в действии!)
    } // конец цикла
    return total; // отдаем итог
} // конец

void Directory::print(int depth) const { // нарисовать папку в консоли
    std::cout << std::string(depth * 4, ' ') // отступы (пробелы * глубину)
              << "|-- [ПАПКА] " << getName() // выводим имя
              << " [" << getCreationDate().toString() << "]\n"; // и дату
    for (const auto& child : children) { // бежим по детям
        child->print(depth + 1); // рекурсивно просим их напечатать себя, увеличивая отступ на 1
    } // конец цикла
} // конец печати
