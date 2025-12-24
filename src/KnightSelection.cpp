#include "KnightSelection.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iostream>

KnightSelection::KnightSelection(int totalKnights, int requiredKnights)
    : totalKnights(totalKnights)
    , requiredKnights(requiredKnights)
    , selected(totalKnights, false)
    , raisedHand(totalKnights, false)
    , selectedCount(0)
    , selectionFinished(false)
    , gen(rd())
{
    if (totalKnights <= 0 || requiredKnights <= 0 || requiredKnights > totalKnights) {
        throw std::invalid_argument("Invalid number of knights");
    }
}

std::vector<int> KnightSelection::getNeighbors(int id) const {
    std::vector<int> neighbors;
    
    int leftNeighbor = (id - 1 + totalKnights) % totalKnights;
    int rightNeighbor = (id + 1) % totalKnights;
    
    neighbors.push_back(leftNeighbor);
    neighbors.push_back(rightNeighbor);
    
    return neighbors;
}

bool KnightSelection::canRaiseHand(int id) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    if (selected[id] || raisedHand[id]) {
        return false;
    }
    
    auto neighbors = getNeighbors(id);
    for (int neighbor : neighbors) {
        if (raisedHand[neighbor]) {
            return false;
        }
    }
    
    return true;
}

int KnightSelection::selectRandomAvailableKnight() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<int> availableKnights;
    
    for (int i = 0; i < totalKnights; ++i) {
        if (!selected[i] && !raisedHand[i]) {
            auto neighbors = getNeighbors(i);
            bool neighborRaised = false;
            for (int neighbor : neighbors) {
                if (raisedHand[neighbor]) {
                    neighborRaised = true;
                    break;
                }
            }
            if (!neighborRaised) {
                availableKnights.push_back(i);
            }
        }
    }
    
    if (availableKnights.empty()) {
        return -1;
    }
    
    std::uniform_int_distribution<> dis(0, availableKnights.size() - 1);
    return availableKnights[dis(gen)];
}

void KnightSelection::knightThread(int id) {
    while (!selectionFinished && selectedCount < requiredKnights) {
        // Рыцарь пытается поднять руку
        if (canRaiseHand(id)) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (!selected[id] && !selectionFinished) {
                    raisedHand[id] = true;
                }
            }
            
            // Короткая пауза
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            // Проверяем, не выбран ли рыцарь
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (selected[id]) {
                    raisedHand[id] = false;
                }
            }
        }
        
        // Пауза перед следующей попыткой
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void KnightSelection::startSelection() {
    std::cout << "Starting knight selection" << std::endl;
    std::cout << "Total knights: " << totalKnights << std::endl;
    std::cout << "Required to select: " << requiredKnights << std::endl;
    
    // Запускаем потоки для рыцарей
    std::vector<std::thread> knights;
    for (int i = 0; i < totalKnights; ++i) {
        knights.emplace_back(&KnightSelection::knightThread, this, i);
    }
    
    // Основной цикл выбора
    int attempts = 0;
    const int maxAttempts = 1000;
    
    while (selectedCount < requiredKnights && attempts < maxAttempts) {
        attempts++;
        
        // Выбираем случайного доступного рыцаря
        int knightId = selectRandomAvailableKnight();
        
        if (knightId != -1) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                
                // Проверяем, что рыцарь все еще доступен
                if (!selected[knightId] && !raisedHand[knightId]) {
                    // Проверяем соседей
                    auto neighbors = getNeighbors(knightId);
                    bool neighborRaised = false;
                    for (int neighbor : neighbors) {
                        if (raisedHand[neighbor]) {
                            neighborRaised = true;
                            break;
                        }
                    }
                    
                    if (!neighborRaised) {
                        selected[knightId] = true;
                        raisedHand[knightId] = false;
                        selectedCount++;
                        
                        std::cout << "Knight " << knightId << " selected for the mission" << std::endl;
                        std::cout << "Selected: " << selectedCount << " of " << requiredKnights << std::endl;
                        
                        // Опускаем руки соседей
                        for (int neighbor : neighbors) {
                            raisedHand[neighbor] = false;
                        }
                    }
                }
            }
        } else {
            // Нет доступных рыцарей - сбрасываем все руки
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::fill(raisedHand.begin(), raisedHand.end(), false);
            }
        }
        
        // Пауза между итерациями выбора
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Завершаем выбор
    {
        std::lock_guard<std::mutex> lock(mtx);
        selectionFinished = true;
    }
    
    // Ждем завершения всех потоков
    for (auto& knight : knights) {
        if (knight.joinable()) {
            knight.join();
        }
    }
    
    // Проверяем результат
    if (selectedCount >= requiredKnights) {
        std::cout << "Selection completed successfully" << std::endl;
    } else {
        std::cout << "Selection failed after " << attempts << " attempts" << std::endl;
        std::cout << "Selected only " << selectedCount << " knights" << std::endl;
    }
}

void KnightSelection::printSelectedKnights() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    std::cout << "Selected knights: ";
    bool first = true;
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) {
            if (!first) std::cout << ", ";
            std::cout << i;
            first = false;
        }
    }
    std::cout << std::endl;
}

std::vector<int> KnightSelection::getSelectedKnights() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    std::vector<int> result;
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) {
            result.push_back(i);
        }
    }
    return result;
}

bool KnightSelection::validateSelection() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto selectedKnights = getSelectedKnights();
    
    if (selectedKnights.size() != static_cast<size_t>(requiredKnights)) {
        std::cerr << "Error: selected " << selectedKnights.size() 
                  << " knights instead of " << requiredKnights << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < selectedKnights.size(); ++i) {
        for (size_t j = i + 1; j < selectedKnights.size(); ++j) {
            int diff = std::abs(selectedKnights[i] - selectedKnights[j]);
            int circularDiff = std::min(diff, totalKnights - diff);
            
            if (circularDiff == 1) {
                std::cerr << "Error: knights " << selectedKnights[i] 
                          << " and " << selectedKnights[j] << " are neighbors" << std::endl;
                return false;
            }
        }
    }
    
    return true;
}
