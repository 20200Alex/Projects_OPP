#ifndef CITY_CAPTURE_HPP
#define CITY_CAPTURE_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

class CityCapture {
public:
    // Конструктор принимает количество городов (должно быть 20)
    CityCapture(int num_cities = 20);
    
    // Запуск симуляции захвата городов
    void simulateCapture();
    
    // Получение результатов (для главного процесса)
    std::map<int, std::vector<int>> getCaptureResults() const;
    
    // Получение шифров (для главного процесса)
    std::map<int, std::vector<int>> getCipherResults() const;
    
    // Вывод результатов на экран
    void printResults() const;
    
    // Проверка корректности результатов
    bool validateResults() const;
    
private:
    int num_cities_;                    // Количество городов (20)
    int world_size_;                    // Общее количество MPI процессов
    int world_rank_;                    // Ранг текущего процесса
    
    // Данные процесса (города)
    std::vector<int> captured_cities_;  // Захваченные города данным процессом
    std::vector<int> cipher_parts_;     // Части шифра у данного города
    
    // Метод для главного процесса
    void masterProcess();
    
    // Метод для процессов-городов
    void cityProcess();
    
    // Генерация части шифра для города
    int generateCipherPart(int city_id) const;
    
    // Логирование события
    void logEvent(const std::string& event) const;
    
    // Вспомогательные методы MPI
    void broadcastToAllCities(const std::vector<int>& data, int tag);
    void gatherFromAllCities(std::vector<int>& data, int tag);
    void sendCipherPart(int dest_city, int cipher_part);
    void receiveCipherPart(int source_city);
};

#endif // CITY_CAPTURE_HPP
