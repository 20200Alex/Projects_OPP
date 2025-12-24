#include "KnightSelection.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

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
        throw std::invalid_argument("Некорректные параметры количества рыцарей");
    }
}

std::vector<int> KnightSelection::getNeighbors(int id) const {
    std::vector<int> neighbors;
    
    // Рыцари сидят за круглым столом
    int leftNeighbor = (id - 1 + totalKnights) % totalKnights;
    int rightNeighbor = (id + 1) % totalKnights;
    
    neighbors.push_back(leftNeighbor);
    neighbors.push_back(rightNeighbor);
    
    return neighbors;
}

bool KnightSelection::canRaiseHand(int id) const {
    // Рыцарь не может поднять руку, если:
    // 1. Он уже выбран
    // 2. Он уже поднял руку
    // 3. Любой из его соседей уже поднял руку
    
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
    std::vector<int> availableKnights;
    
    // Собираем всех рыцарей, которые могут поднять руку
    for (int i = 0; i < totalKnights; ++i) {
        if (canRaiseHand(i)) {
            availableKnights.push_back(i);
        }
    }
    
    if (availableKnights.empty()) {
        return -1;
    }
    
    // Выбираем случайного рыцаря из доступных
    std::uniform_int_distribution<> dis(0, availableKnights.size() - 1);
    return availableKnights[dis(gen)];
}

void KnightSelection::knightThread(int id) {
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    
    while (!selectionFinished && selectedCount < requiredKnights) {
        lock.lock();
        
        // Проверяем, может ли рыцарь поднять руку
        if (canRaiseHand(id)) {
            // Случайная задержка для имитации раздумий
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10 + (id * 7) % 50));
            lock.lock();
            
            // Повторная проверка после задержки
            if (canRaiseHand(id) && !selectionFinished) {
                raisedHand[id] = true;
                std::cout << "Рыцарь " << id << " поднял руку" << std::endl;
                
                // Короткая пауза перед выбором
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                lock.lock();
                
                // Проверяем, выбран ли этот рыцарь
                if (selected[id]) {
                    raisedHand[id] = false;
                }
            }
        }
        
        lock.unlock();
        
        // Небольшая пауза перед следующей попыткой
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void KnightSelection::startSelection() {
    std::cout << "=== Начало выбора рыцарей для спецоперации ===" << std::endl;
    std::cout << "Всего рыцарей: " << totalKnights << std::endl;
    std::cout << "Требуется выбрать: " << requiredKnights << std::endl;
    
    // Запускаем потоки для каждого рыцаря
    std::vector<std::thread> knights;
    for (int i = 0; i < totalKnights; ++i) {
        knights.emplace_back(&KnightSelection::knightThread, this, i);
    }
    
    // Основной цикл выбора рыцарей
    while (selectedCount < requiredKnights) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::lock_guard<std::mutex> lock(mtx);
        
        // Пытаемся выбрать рыцаря
        int knightId = selectRandomAvailableKnight();
        
        if (knightId != -1) {
            // Выбираем этого рыцаря
            selected[knightId] = true;
            raisedHand[knightId] = false;
            selectedCount++;
            
            std::cout << "\n Рыцарь " << knightId << " выбран для похода!" << std::endl;
            std::cout << "Выбрано: " << selectedCount << " из " << requiredKnights << std::endl;
            
            // Опускаем руки соседей выбранного рыцаря
            auto neighbors = getNeighbors(knightId);
            for (int neighbor : neighbors) {
                raisedHand[neighbor] = false;
                std::cout << "  Рыцарь " << neighbor << " опустил руку (сосед выбранного)" << std::endl;
            }
            
            // Проверяем, нужно ли продолжить
            if (selectedCount >= requiredKnights) {
                selectionFinished = true;
                break;
            }
        }
        
        // Если нет доступных рыцарей, сбрасываем все поднятые руки
        if (selectRandomAvailableKnight() == -1) {
            std::cout << "\n Нет доступных рыцарей, сбрасываю все руки..." << std::endl;
            std::fill(raisedHand.begin(), raisedHand.end(), false);
        }
    }
    
    // Помечаем завершение выбора
    selectionFinished = true;
    
    // Ожидаем завершения всех потоков
    for (auto& knight : knights) {
        if (knight.joinable()) {
            knight.join();
        }
    }
    
    std::cout << "\n=== Выбор завершен ===" << std::endl;
}

void KnightSelection::printSelectedKnights() const {
    std::cout << "\n📋 Выбранные рыцари: ";
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
    std::vector<int> result;
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) {
            result.push_back(i);
        }
    }
    return result;
}

bool KnightSelection::validateSelection() const {
    auto selectedKnights = getSelectedKnights();
    
    // Проверяем количество выбранных рыцарей
    if (selectedKnights.size() != static_cast<size_t>(requiredKnights)) {
        std::cerr << "Ошибка: выбрано " << selectedKnights.size() 
                  << " рыцарей вместо " << requiredKnights << std::endl;
        return false;
    }
    
    // Проверяем, что нет соседей среди выбранных
    for (size_t i = 0; i < selectedKnights.size(); ++i) {
        for (size_t j = i + 1; j < selectedKnights.size(); ++j) {
            int diff = std::abs(selectedKnights[i] - selectedKnights[j]);
            int circularDiff = std::min(diff, totalKnights - diff);
            
            // В круглом столе соседи имеют разницу 1 или totalKnights-1
            if (circularDiff == 1) {
                std::cerr << "Ошибка: рыцари " << selectedKnights[i] 
                          << " и " << selectedKnights[j] << " являются соседями!" << std::endl;
                return false;
            }
        }
    }
    
    return true;
}
