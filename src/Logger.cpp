#include "../include/Logger.h" // подключаем заголовочный логгера
#include <ctime> // для времени
#include <iomanip> // для красивого вывода в консоль

void Logger::log(LogLevel level, const std::string& message) { // сама функция логирования
    std::ofstream file("history.log", std::ios_base::app); // открываем файл history.log в режиме дозаписи
    if (!file.is_open()) return; // если не открылся - ну и фиг с ним, выходим

    std::time_t t = std::time(nullptr); // получаем текущее время компа
    std::tm tmp = {}; // создаем структурку для времени
    localtime_s(&tmp, &t); // разбиваем время на часы-минуты (безопасная версия для винды)
    
    file << "[" << std::put_time(&tmp, "%Y-%m-%d %H:%M:%S") << "] "; // пишем дату и время красиво в лог
    
    if (level == LogLevel::INFO) file << "[ИНФО] "; // если инфа - пишем тег инфо
    else if (level == LogLevel::WARNING) file << "[ВАРНИНГ] "; // если варн - пишем варнинг
    else file << "[ОШИБКА] "; // иначе это эррорка
    
    file << message << "\n"; // дописываем само сообщение и делаем перенос строки
} // конец функции
