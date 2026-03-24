#include <iostream> // библиотека для cin cout (вывод текста)
#include <string> // чтоб работать со строками (не char*)
#include <windows.h> // нужно чтоб поставить кодировку винды (для русского языка)
#include "../include/Directory.h" // подключаем наши папки
#include "../include/File.h" // подключаем файлы
#include "../include/Serialization.h" // тулзы чтобы сохранять
#include "../include/SearchAnalytics.h" // тулзы чтобы искать и аудит
#include "../include/Exceptions.h" // свои родные ошибки
#include "../include/Logger.h" // логгер

void printMenu() { // функция, чисто чтоб отрисовать менюшку
    std::cout << "\n--- Продвинутая Файловая Система Алабуга ---\n"; // шапка
    std::cout << "1. Напечатать Дерево\n"; // пункт 1
    std::cout << "2. Создать Файл\n"; // пункт 2
    std::cout << "3. Создать Папку\n"; // пункт 3
    std::cout << "4. Глобальный Аудит\n"; // пункт 4: посмотреть размеры
    std::cout << "5. Экспорт в CSV\n"; // пункт 5: выкачать в эксель
    std::cout << "6. Сохранить в archive.dat (Бинарная Сериализация)\n"; // пункт 6: зберегти
    std::cout << "7. Загрузить из archive.dat (Десериализация)\n"; // пункт 7: лоад
    std::cout << "8. Выход\n"; // валим
    std::cout << "Выберите действие: "; // просим циферку
} // конец

int main() { // точка входа в прогу
    SetConsoleCP(CP_UTF8); // ставим кодовую страницу UTF-8 на ввод консоли (чтоб кириллица была)
    SetConsoleOutputCP(CP_UTF8); // ставим кодовую страницу UTF-8 на вывод (чтоб крякозябр не было)
    setlocale(LC_ALL, ".UTF8"); // локаль для стандартных функций C++

    auto root = std::make_unique<Directory>("root", AccessLevel::ADMIN); // создаем папку Рут с правами админа с самого начала
    Logger::log(LogLevel::INFO, "Приложение запущено"); // сразу пишем в лог

    while (true) { // бесконечный цикл (пока на 8 не нажмут)
        printMenu(); // отрисовываем меню
        int choice; // куда кладем выбор
        if (!(std::cin >> choice)) { // если юзер ввел букву вместо цифры (cin ломается)
            std::cin.clear(); // лечим объект cin
            std::cin.ignore(10000, '\n'); // очищаем буфер мусора до самого 엔тера (скипаем 10к символов)
            continue; // идем на новый круг цикла, заставляем вводить заново
        } // конец ифа

        try { // открываем блок защиты от ошибок (эксепшенов)
            if (choice == 1) { // если ввели 1
                root->print(); // печатаем все дерево (root сам найдет и напечатает всех детей)
            } else if (choice == 2) { // если ввели 2 (создать файл)
                std::string name, ext; // переменные под имя и формат
                size_t size; // размер файлов
                std::cout << "Имя: "; std::cin >> name; // читаем имя
                std::cout << "Расширение: "; std::cin >> ext; // читаем формат
                std::cout << "Размер (байты): "; 
                if (!(std::cin >> size)) { // читаем сколько весит и проверяем на буквы
                    std::cin.clear(); std::cin.ignore(10000, '\n'); // чистим мусор
                    throw FileSystemException("Размер должен быть числом!"); // кидаем ошибку
                }
                root->addResource(std::make_unique<File>(name, ext, size)); // создаем уникальный указатель на файл и пушим в родителя
                std::cout << "Файл успешно создан.\n"; // говорим что всё чиназес
            } else if (choice == 3) { // если 3 (создать папку)
                std::string name; // имя
                int level; // права доступа цифрой
                std::cout << "Имя: "; std::cin >> name; // читаем имя
                std::cout << "Уровень доступа (0-Гость, 1-Пользователь, 2-Админ): ";
                if (!(std::cin >> level) || level < 0 || level > 2) { // читаем уровень и проверяем корректность
                    std::cin.clear(); std::cin.ignore(10000, '\n'); // чистим мусор
                    throw FileSystemException("Уровень доступа должен быть числом от 0 до 2!"); // кидаем ошибку
                }
                root->addResource(std::make_unique<Directory>(name, static_cast<AccessLevel>(level))); // создаем уникальную Папку (кастуем число в правильный Enum)
                std::cout << "Папка успешно создана.\n"; // отчитываемся
            } else if (choice == 4) { // если 4 (аудит)
                SearchAnalytics::globalAudit(root.get()); // кидаем в глобал аудит сырой указатель (get) от нашего уника
            } else if (choice == 5) { // если 5 (экспорт)
                SearchAnalytics::exportToCSV(root.get(), "export.csv"); // выкачиваем всё в файлик export.csv
                std::cout << "Структура экспортирована в export.csv\n"; // выводим инфо
            } else if (choice == 6) { // если 6 (сохранить стейт)
                Serialization::saveToFile(root.get(), "archive.dat"); // закатываем весь корень в archive.dat
                std::cout << "Успешно сохранено в бинарный архив archive.dat\n"; // радуемся
            } else if (choice == 7) { // если 7 (загрузить стейт)
                root = Serialization::loadFromFile("archive.dat"); // переприсваиваем новый распакованный Рут поверх старого (старый автоматом удалится из памяти)
                std::cout << "Успешно загружено из архива archive.dat\n"; // круто
            } else if (choice == 8) { // если 8 - пора валить
                break; // вырываемся из бесконечного цикла
            } else { // если чушпан ввел 9 или 10
                std::cout << "Неверный выбор. Повторите попытку.\n"; // ругаемся
            } // конец цепочки if-else
        } catch (const FileSystemException& e) { // ловим наши кастомные логические баги (FileSystemException - например, спецсимволы в имени)
            std::cout << "Ошибка Файловой Системы: " << e.what() << "\n"; // выплевываем что именно не так в лицо
            Logger::log(LogLevel::ERR, e.what()); // тихо пишем это в лог-файл (чтобы админ видел)
        } catch (const std::exception& e) { // если случилась вообще лютая дичь из std (типа out_of_range)
            std::cout << "Критическая ошибка: " << e.what() << "\n"; // выводим критическое падение
        } // конец блока try-catch
    } // конец while

    Logger::log(LogLevel::INFO, "Приложение остановлено"); // перед закрытием софта прощаемся в логах
    return 0; // прога завершилась без крашей (код 0)
} // конец 메인
