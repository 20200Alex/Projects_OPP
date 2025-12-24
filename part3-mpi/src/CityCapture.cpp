#include "CityCapture.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <mpi.h>

CityCapture::CityCapture(int num_cities) 
    : num_cities_(num_cities) {
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size_);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);
    
    if (world_rank_ == 0) {
        std::cout << "=== City Capture Simulation ===" << std::endl;
        std::cout << "Number of cities: " << num_cities_ << std::endl;
        std::cout << "MPI processes: " << world_size_ << std::endl;
        
        if (world_size_ != num_cities_ + 1) {
            std::cerr << "Warning: Need exactly " << (num_cities_ + 1) 
                      << " MPI processes (cities + commander)" << std::endl;
        }
    }
}

void CityCapture::simulateCapture() {
    if (world_rank_ == 0) {
        masterProcess();
    } else if (world_rank_ <= num_cities_) {
        cityProcess();
    } else {
        // Лишние процессы не участвуют
        if (world_rank_ == world_size_ - 1) {
            std::cout << "Process " << world_rank_ << " is idle (not needed)" << std::endl;
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
}

void CityCapture::masterProcess() {
    std::cout << "\nCommander process starting simulation..." << std::endl;
    
    // Создаем порядок захвата городов (случайная перестановка)
    std::vector<int> capture_order(num_cities_);
    for (int i = 0; i < num_cities_; ++i) {
        capture_order[i] = i + 1; // Города нумеруются с 1
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(capture_order.begin(), capture_order.end(), g);
    
    std::cout << "\nCapture order: ";
    for (int city : capture_order) {
        std::cout << city << " ";
    }
    std::cout << std::endl;
    
    // Отправляем порядок захвата всем городам
    MPI_Bcast(capture_order.data(), num_cities_, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Симуляция захвата городов
    std::vector<int> captured_cities;
    std::vector<int> all_ciphers(num_cities_);
    
    for (int step = 0; step < num_cities_; ++step) {
        int current_city = capture_order[step];
        
        logEvent("Step " + std::to_string(step + 1) + 
                 ": Capturing city " + std::to_string(current_city));
        
        // Оповещаем город о захвате
        MPI_Send(&step, 1, MPI_INT, current_city, 0, MPI_COMM_WORLD);
        
        // Получаем часть шифра от захваченного города
        int cipher_part;
        MPI_Recv(&cipher_part, 1, MPI_INT, current_city, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        logEvent("Received cipher part " + std::to_string(cipher_part) +
                 " from city " + std::to_string(current_city));
        
        // Сохраняем часть шифра
        all_ciphers[step] = cipher_part;
        
        // Отправляем всем уже захваченным городам новую часть шифра
        for (int captured_city : captured_cities) {
            MPI_Send(&cipher_part, 1, MPI_INT, captured_city, 
                    2 + step, MPI_COMM_WORLD);
        }
        
        // Получаем от первого захваченного города новую часть шифра
        if (!captured_cities.empty()) {
            int new_cipher_part;
            MPI_Recv(&new_cipher_part, 1, MPI_INT, captured_cities[0], 
                    3 + step, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            logEvent("First city " + std::to_string(captured_cities[0]) +
                     " sent new cipher part " + std::to_string(new_cipher_part));
            
            // Рассылаем новую часть всем захваченным городам
            for (int captured_city : captured_cities) {
                if (captured_city != captured_cities[0]) {
                    MPI_Send(&new_cipher_part, 1, MPI_INT, captured_city, 
                            4 + step, MPI_COMM_WORLD);
                }
            }
            
            // Отправляем новую часть текущему городу
            MPI_Send(&new_cipher_part, 1, MPI_INT, current_city, 
                    5 + step, MPI_COMM_WORLD);
        }
        
        captured_cities.push_back(current_city);
        
        // Небольшая задержка для реалистичности
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    // Собираем полные шифры от всех городов
    std::vector<int> complete_ciphers(num_cities_ * num_cities_);
    for (int i = 1; i <= num_cities_; ++i) {
        std::vector<int> city_cipher(num_cities_);
        MPI_Recv(city_cipher.data(), num_cities_, MPI_INT, i, 
                99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        std::copy(city_cipher.begin(), city_cipher.end(), 
                  complete_ciphers.begin() + (i - 1) * num_cities_);
    }
    
    // Отправляем сигнал о завершении всем городам
    int finish_signal = -1;
    for (int i = 1; i <= num_cities_; ++i) {
        MPI_Send(&finish_signal, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
}

void CityCapture::cityProcess() {
    int city_id = world_rank_;
    
    logEvent("City " + std::to_string(city_id) + " initialized");
    
    // Получаем порядок захвата
    std::vector<int> capture_order(num_cities_);
    MPI_Bcast(capture_order.data(), num_cities_, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Генерируем свою часть шифра
    int my_cipher_part = generateCipherPart(city_id);
    cipher_parts_.push_back(my_cipher_part);
    
    // Ждем команды от командующего
    while (true) {
        int step;
        MPI_Recv(&step, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        if (step == -1) { // Сигнал завершения
            break;
        }
        
        // Проверяем, наш ли это ход захвата
        int current_city = capture_order[step];
        
        if (current_city == city_id) {
            // Нас захватывают
            logEvent("City " + std::to_string(city_id) + " captured at step " + 
                     std::to_string(step + 1));
            
            // Отправляем нашу часть шифра командующему
            MPI_Send(&my_cipher_part, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            
            // Ждем получения всех частей шифра
            for (int i = 0; i < step; ++i) {
                int cipher_part;
                MPI_Recv(&cipher_part, 1, MPI_INT, 0, 2 + step, 
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cipher_parts_.push_back(cipher_part);
            }
            
            // Если мы первый захваченный город, отправляем новую часть шифра
            if (step == 0) {
                int new_cipher_part = generateCipherPart(city_id * 100 + step);
                MPI_Send(&new_cipher_part, 1, MPI_INT, 0, 3 + step, MPI_COMM_WORLD);
            } else {
                // Ждем новую часть от первого города
                int new_cipher_part;
                MPI_Recv(&new_cipher_part, 1, MPI_INT, 0, 4 + step, 
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cipher_parts_.push_back(new_cipher_part);
            }
        } else {
            // Нас уже захватили ранее
            // Получаем часть шифра от нового города
            int new_cipher_part;
            MPI_Recv(&new_cipher_part, 1, MPI_INT, 0, 2 + step, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cipher_parts_.push_back(new_cipher_part);
            
            // Если мы первый город, отправляем новую часть
            if (city_id == capture_order[0]) {
                int our_new_part = generateCipherPart(city_id * 100 + step);
                MPI_Send(&our_new_part, 1, MPI_INT, 0, 3 + step, MPI_COMM_WORLD);
            } else {
                // Получаем новую часть от первого города
                int newest_cipher_part;
                MPI_Recv(&newest_cipher_part, 1, MPI_INT, 0, 4 + step, 
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cipher_parts_.push_back(newest_cipher_part);
            }
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    // Отправляем полный шифр командующему
    MPI_Send(cipher_parts_.data(), cipher_parts_.size(), MPI_INT, 0, 99, MPI_COMM_WORLD);
    
    logEvent("City " + std::to_string(city_id) + " complete cipher size: " + 
             std::to_string(cipher_parts_.size()));
}

int CityCapture::generateCipherPart(int city_id) const {
    // Генерация уникальной части шифра на основе ID города
    std::hash<int> hasher;
    auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    return static_cast<int>((hasher(city_id) ^ hasher(timestamp)) % 1000 + 1000);
}

void CityCapture::logEvent(const std::string& event) const {
    if (world_rank_ == 0) {
        std::cout << "[Commander] " << event << std::endl;
    } else if (world_rank_ <= num_cities_) {
        std::stringstream ss;
        ss << "[City " << std::setw(2) << world_rank_ << "] " << event;
        std::cout << ss.str() << std::endl;
    }
}

std::map<int, std::vector<int>> CityCapture::getCaptureResults() const {
    // Этот метод вызывается только из главного процесса после симуляции
    std::map<int, std::vector<int>> results;
    // Реализация будет зависеть от того, как хранятся данные
    return results;
}

std::map<int, std::vector<int>> CityCapture::getCipherResults() const {
    // Этот метод вызывается только из главного процесса после симуляции
    std::map<int, std::vector<int>> results;
    // Реализация будет зависеть от того, как хранятся данные
    return results;
}

void CityCapture::printResults() const {
    if (world_rank_ == 0) {
        std::cout << "\n=== Final Results ===" << std::endl;
        std::cout << "All cities should now have complete cipher." << std::endl;
    }
}

bool CityCapture::validateResults() const {
    if (world_rank_ == 0) {
        // Главный процесс проверяет, что все города получили полный шифр
        std::vector<int> cipher_sizes(num_cities_);
        
        for (int i = 1; i <= num_cities_; ++i) {
            MPI_Recv(&cipher_sizes[i-1], 1, MPI_INT, i, 101, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        bool all_complete = true;
        for (int size : cipher_sizes) {
            if (size != num_cities_) {
                all_complete = false;
                break;
            }
        }
        
        return all_complete;
    } else if (world_rank_ <= num_cities_) {
        // Города отправляют размер своего шифра
        int cipher_size = cipher_parts_.size();
        MPI_Send(&cipher_size, 1, MPI_INT, 0, 101, MPI_COMM_WORLD);
        return cipher_size == num_cities_;
    }
    
    return false;
}
