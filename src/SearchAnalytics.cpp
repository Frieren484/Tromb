#include "../include/SearchAnalytics.h" // хедер аналитики
#include "../include/Exceptions.h" // эксепшены
#include "../include/Utils.h" // утилиты (чистилка CSV)
#include <iostream> // для вывода
#include <fstream> // для записи в файл

// Вспомогательная рекурсивная функция поиска
void searchHelper(Directory* current, const SearchCriteria& criteria, std::vector<Resource*>& results) { // это чисто внутренний метод
    for (const auto& child : current->getChildren()) { // бежим по всем детям в этой папке
        bool match = true; // изначально думаем, что файл нам подходит

        if (!criteria.nameMask.empty() && // если маска не пустая
            child->getName().find(criteria.nameMask) == std::string::npos) { // и кусок текста не найден в имени
            match = false; // не подходит, отбой
        } // конец ифа

        if (match && !criteria.extension.empty()) { // если всё еще подходит, но задано расширение
            File* filePtr = dynamic_cast<File*>(child.get()); // пытаемся по-умному привести к Файлу
            if (filePtr) { // если это реально файл
                if (filePtr->getExtension() != criteria.extension) { // и расширение не совпало
                    match = false; // отбой
                } // конец ифа
            } else { // если это папка, а мы ищем расщирение
                match = false; // папки не имеют расширения, отбой
            } // конец
        } // конец

        std::time_t cDate = child->getCreationDate().value; // берем время создания
        if (match && criteria.startDate != 0 && cDate < criteria.startDate) match = false; // если создано раньше чем надо - отбой
        if (match && criteria.endDate != -1 && cDate > criteria.endDate) match = false; // если создано позже чем надо - отбой

        if (match) { // если прошел все проверки
            results.push_back(child.get()); // запихиваем сырой указатель в результаты
        } // конец ифа

        Directory* subDir = dynamic_cast<Directory*>(child.get()); // пытаемся понять, папка ли это
        if (subDir) { // если да, это папка
            searchHelper(subDir, criteria, results); // ПРЫГАЕМ ВГЛУБЬ (рекурсия)
        } // конец ифа
    } // конец обхода детей
} // конец хелпера

std::vector<Resource*> SearchAnalytics::findResources(Directory* root, const SearchCriteria& criteria) { // красивый метод с интерфейсом
    std::vector<Resource*> results; // создаем пустой ответ
    searchHelper(root, criteria, results); // просим хелпера сделать всю грязную работу
    return results; // отдаем результат
} // конец метода

// Вспомогательная функция для аудита
void globalAuditHelper(Directory* current, size_t& fileCount, size_t& dirCount, size_t& totalSize) { // рекурсивный счетчик статистики
    for (const auto& child : current->getChildren()) { // бежим по детям
        totalSize += child->calculateSize(); // суммируем вес
        
        if (dynamic_cast<File*>(child.get())) { // если это файл
            fileCount++; // +1 к файлам
        } else if (Directory* subDir = dynamic_cast<Directory*>(child.get())) { // если папка
            dirCount++; // +1 к папкам
            globalAuditHelper(subDir, fileCount, dirCount, totalSize); // рекурсивно считаем внутри
        } // конец ифа
    } // конец цикла
} // конец хелпера

void SearchAnalytics::globalAudit(Directory* root) { // красивый фасад аудита
    size_t fileCount = 0; // счетчик файлов
    size_t dirCount = 0; // счетчик папок
    size_t totalSize = 0; // общий вес

    globalAuditHelper(root, fileCount, dirCount, totalSize); // просим хелпера посчитать

    std::cout << "--- Глобальный Аудит ---\n"; // выводим шапку
    std::cout << "Папки: " << dirCount << "\n"; // выводим папки
    std::cout << "Файлы: " << fileCount << "\n"; // выводим файлы
    std::cout << "Общий размер: " << totalSize << " байт\n"; // выводим вес
    std::cout << "Средний размер файла: " << (fileCount > 0 ? (totalSize / fileCount) : 0) << " байт\n"; // считаем среднее: вес делим на кол-во файлов
} // конец метода

void exportHelper(Directory* dir, std::ofstream& out, const std::string& currentPath) { // хелпер для сохранения дерева в эксель
    for (const auto& child : dir->getChildren()) { // обходим детей
        std::string path = currentPath + "/" + Utils::sanitizeCSV(child->getName()); // склеиваем полный путь (типа root/system/log)
        std::string type = "DIR"; // по дефолту пишем что это папка
        std::string ext = ""; // расширения нет
        size_t size = child->calculateSize(); // считаем вес

        if (File* f = dynamic_cast<File*>(child.get())) { // если это на самом деле файл
            type = "FILE"; // меняем тип на файл
            ext = Utils::sanitizeCSV(f->getExtension()); // достаем очищенное расширение
        } // конец ифа

        out << type << "|" << path << "|" << ext << "|" << size << "|" << child->getCreationDate().toString() << "\n"; // пишем строчку в файл (каждое поле через палочку)

        if (Directory* subDir = dynamic_cast<Directory*>(child.get())) { // если это папка
            exportHelper(subDir, out, path); // погружаемся внутрь рекурсией
        } // конец
    } // конец обхода
} // конец экспорта хелпера

void SearchAnalytics::exportToCSV(Directory* root, const std::string& filename) { // красивый интерфейс вызова экспорта
    std::ofstream out(filename); // открываем файлик на запись
    if (!out.is_open()) { // если не открылся
        throw FileSystemException("Не удалось открыть файл на экспорт: " + filename); // кидаем ошибку в ебало
    } // конец
    
    out << "Type|Path|Extension|Size|CreationDate\n"; // пишем заголовки столбцов как в экселе
    exportHelper(root, out, "root"); // запускаем обход дерева
} // конец метода
