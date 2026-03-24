#include "../include/Serialization.h" // тянем хедер сериализации
#include "../include/File.h" // файл
#include "../include/Exceptions.h" // ошибки
#include "../include/Logger.h" // логи
#include <fstream> // для файлов (чтение и запись)
#include <iostream> // вывод в консоль

const uint32_t MAGIC_NUMBER = 0xAF1BE55F; // секретное число для защиты файлов от подделки (типа пароль архива)
const uint8_t TYPE_FILE = 1; // числовой идентификатор что это файл
const uint8_t TYPE_DIR = 2; // идентификатор что это папка

void writeString(std::ofstream& out, const std::string& str) { // хелпер: записать строку в бинарник (чтоб читалось норм)
    size_t len = str.length(); // узнаем длину текста
    out.write(reinterpret_cast<const char*>(&len), sizeof(len)); // сначала пишем длину текста (размер в байтах)
    out.write(str.c_str(), len); // потом сам текст (посимвольно)
} // конец хелпера

std::string readString(std::ifstream& in) { // хелпер: достать строку из бинарника
    size_t len; // переменная под длину
    in.read(reinterpret_cast<char*>(&len), sizeof(len)); // сперва читаем сколько символов там лежит
    std::string str(len, '\0'); // создаем пустую строку нужного размера (с нулями)
    in.read(&str[0], len); // читаем прямо поверх нулей сам текст
    return str; // отдаем готовую строку
} // конец

void serializeResource(std::ofstream& out, Resource* res) { // рекурсивная бетономешалка для сохранения узлов ФС
    if (File* f = dynamic_cast<File*>(res)) { // проверяем, не Файл ли это
        out.write(reinterpret_cast<const char*>(&TYPE_FILE), sizeof(TYPE_FILE)); // пишем тип (1 = ФАЙЛ)
        writeString(out, f->getName()); // пишем имя
        writeString(out, f->getExtension()); // пишем формат (txt, exe)
        
        size_t size = f->getSize(); // берем вес
        out.write(reinterpret_cast<const char*>(&size), sizeof(size)); // пишем вес
        
        std::time_t cDate = f->getCreationDate().value; // берем время создания
        out.write(reinterpret_cast<const char*>(&cDate), sizeof(cDate)); // пишем время
    } else if (Directory* d = dynamic_cast<Directory*>(res)) { // если это Папка
        out.write(reinterpret_cast<const char*>(&TYPE_DIR), sizeof(TYPE_DIR)); // пишем тип (2 = ПАПКА)
        writeString(out, d->getName()); // пишем имя папки
        
        AccessLevel acl = d->getAccessLevel(); // берем права доступа
        out.write(reinterpret_cast<const char*>(&acl), sizeof(acl)); // сохраняем права
        
        std::time_t cDate = d->getCreationDate().value; // берем время создания
        out.write(reinterpret_cast<const char*>(&cDate), sizeof(cDate)); // пишем время
        
        const auto& children = d->getChildren(); // берем всех детей в папке
        size_t childCount = children.size(); // считаем сколько там детей
        out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount)); // записываем число детей (чтоб при загрузке цикл знал, сколько крутить)
        
        for (const auto& child : children) { // бежим по всем детям
            serializeResource(out, child.get()); // рекурсивно пишем туда каждого ребенка (вглубь дерева)
        } // конец цикла
    } // конец ифа
} // конец сериализации

std::unique_ptr<Resource> deserializeResource(std::ifstream& in) { // считываем узел обратно из сырых байтов
    uint8_t type; // переменная под тип (1 - файл, 2 - папка)
    in.read(reinterpret_cast<char*>(&type), sizeof(type)); // читаем один байт (тип)
    
    if (type == TYPE_FILE) { // если загружаем ФАЙЛ
        std::string name = readString(in); // достаем имя
        std::string ext = readString(in); // достаем расширение
        
        size_t size; // переменная под размер
        in.read(reinterpret_cast<char*>(&size), sizeof(size)); // читаем размер
        
        std::time_t cDate; // переменная под время
        in.read(reinterpret_cast<char*>(&cDate), sizeof(cDate)); // читаем время
        
        auto file = std::make_unique<File>(name, ext, size); // собираем Файл через умный указатель
        // время мы тут заново не сетим, так как конструктор ставит текущее, но в реале надо бы перезаписать // (пометка студента) //
        return file; // отдаем собранный файл
    } else if (type == TYPE_DIR) { // если загружаем ПАПКУ
        std::string name = readString(in); // читаем название папки
        
        AccessLevel acl; // под права
        in.read(reinterpret_cast<char*>(&acl), sizeof(acl)); // читаем права
        
        std::time_t cDate; // время
        in.read(reinterpret_cast<char*>(&cDate), sizeof(cDate)); // читаем
        
        auto dir = std::make_unique<Directory>(name, acl); // собираем пустую папку
        
        size_t childCount; // сколько в ней детей
        in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount)); // считываем число детей
        
        for (size_t i = 0; i < childCount; ++i) { // крутим цикл столько раз, сколько лежало детей внутри
            dir->addResource(deserializeResource(in)); // рекурсивно прыгаем глубже читать детей и сразу добавляем в папку!
        } // конец
        
        return dir; // отдаем собранную ветку
    } // конец ифа
    
    throw FileSystemException("Обнаружен неизвестный тип ресурса при десериализации."); // если тип ни 1 ни 2, значит файл битый - кидаем эксепшен
} // конец десериализации

void Serialization::saveToFile(Directory* root, const std::string& filename) { // интерфейс сохранить в файл
    std::ofstream out(filename, std::ios::binary); // открываем файл в бинарном режиме (чтоб нули не резались)
    if (!out.is_open()) { // если почему-то не открылось (например нет прав на диск)
        throw FileSystemException("Не удалось открыть файл для записи: " + filename); // кидаем в рожу ошибку
    } // конец
    
    out.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER)); // первым делом пишем МАГИЧЕСКОЕ ЧИСЛО (пароль файла 0xAF1BE55F)
    serializeResource(out, root); // просим хелпера закатать туда всё дерево начиная с корня
    Logger::log(LogLevel::INFO, "Файловая система успешно сериализована в " + filename); // пишем в лог, что всё ок
} // конец

std::unique_ptr<Directory> Serialization::loadFromFile(const std::string& filename) { // интерфейс загрузки
    std::ifstream in(filename, std::ios::binary); // открываем бинарник на чтение
    if (!in.is_open()) { // если не нашли
        throw FileSystemException("Не удалось открыть файл для чтения: " + filename); // кидаем ошибку
    } // конец
    
    uint32_t magic; // переменная под пароль
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic)); // считываем самые первые 4 байта файла
    
    if (magic != MAGIC_NUMBER) { // если они не совпали с нашим паролем (кто-то лазил хекс редактором или это вообще мп3 файл)
        throw SecurityException("Проверка целостности провалена: неверное магическое число в " + filename); // блочим нахрен (SecurityException!)
    } // супер защита
    
    auto rootRes = deserializeResource(in); // запускаем мясорубку по сборке объектов из байтов
    auto rootDir = std::unique_ptr<Directory>(dynamic_cast<Directory*>(rootRes.release())); // перегоняем общий Ресурс в конкретно Папку
    
    if (!rootDir) { // если внезапно корневой элемент оказался Файлом (бред какой-то)
        throw FileSystemException("Корневой элемент в архиве не является директорией."); // кидаем ошибку
    } // конец
    
    Logger::log(LogLevel::INFO, "Файловая система успешно загружена из " + filename); // пишем в лог
    return rootDir; // отдаем воссозданную матрицу
} // всё, готово
