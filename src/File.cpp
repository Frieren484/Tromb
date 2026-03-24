#include "../include/File.h" // подключаем заголовок
#include <iostream> // для std::cout

File::File(const std::string& name, const std::string& ext, size_t initialSize) // конструктор файла
    : Resource(name), extension(ext), size(initialSize) {} // вызываем конструктор бати и свои поля

const std::string& File::getExtension() const { // геттер расширения
    return extension; // возвращаем значение
} // конец метода

void File::setExtension(const std::string& ext) { // сеттер расширения
    extension = ext; // перезаписываем
} // конец метода

size_t File::getSize() const { // функция получить размер
    return size; // возвращаем size
} // конец функции

void File::setSize(size_t newSize) { // сеттер размера файла
    size = newSize; // просто обновляем поле
} // конец

std::unique_ptr<Resource> File::clone() const { // создание глубокой копии (клонирование)
    return std::make_unique<File>(getName(), extension, size); // создаем новый уникальный указатель на точно такой же файл
} // конец метода

size_t File::calculateSize() const { // метод для расчёта (чтоб полиморфизм работал)
    return size; // для файла это просто его размер
} // конец функции

void File::print(int depth) const { // вывести फाइल в терминал
    std::cout << std::string(depth * 4, ' ') // отступы (пробелы) для красивого дерева
              << "|-- [ФАЙЛ] " << getName() // пишем ФАЙЛ и название
              << (extension.empty() ? "" : "." + extension) // если есть расширение - лепим точку, иначе пустоту
              << " (" << size << " байт) [" << getCreationDate().toString() << "]\n"; // выводим размер и дату
} // конец печати
