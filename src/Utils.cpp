#include "../include/Utils.h" // подключаем хедер утилит

bool Utils::validateName(const std::string& name) { // функция проверки имени файла/папки
    std::regex invalidChars("[\\\\/:*?\"<>|]"); // регулярка на запрещенные в винде символы
    if (std::regex_search(name, invalidChars)) { // если находим плохой символ
        throw FileSystemException("Имя содержит недопустимые спецсимволы."); // кидаем нашей ошибкой в лицо
    } // конец ифа
    return true; // если всё чисто - возвращаем ок
} // конец функции

std::string Utils::sanitizeCSV(const std::string& input) { // чистилка для CSV файла
    std::string result = input; // копируем строку
    for (char& c : result) { // бежим по каждому символу
        if (c == '|' || c == '\n' || c == '\r') { // если это разделитель или перенос
            c = '_'; // меняем его на нижнее подчеркивание чтоб не сломать таблицу
        } // конец ифа
    } // конец цикла
    return result; // возвращаем чистенькую строку
} // конец функции
