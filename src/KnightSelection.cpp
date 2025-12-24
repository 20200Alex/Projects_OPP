#include "KnightSelection.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>

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
    
    for (int i = 0; i < totalKnights; ++i) {
        if (canRaiseHand(i)) {
            availableKnights.push_back(i);
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
        {
            std::lock_guard<std::mutex> lock(mtx);
            
            if (canRaiseHand(id) && !selectionFinished) {
                raisedHand[id] = true;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void KnightSelection::startSelection() {
    std::cout << "Starting knight selection" << std::endl;
    std::cout << "Total knights: " << totalKnights << std::endl;
    std::cout << "Required to select: " << requiredKnights << std::endl;
    
    std::vector<std::thread> knights;
    for (int i = 0; i < totalKnights; ++i) {
        knights.emplace_back(&KnightSelection::knightThread, this, i);
    }
    
    while (selectedCount < requiredKnights) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        std::lock_guard<std::mutex> lock(mtx);
        
        int knightId = selectRandomAvailableKnight();
        
        if (knightId != -1) {
            selected[knightId] = true;
            raisedHand[knightId] = false;
            selectedCount++;
            
            std::cout << "Knight " << knightId << " selected for the mission" << std::endl;
            std::cout << "Selected: " << selectedCount << " of " << requiredKnights << std::endl;
            
            auto neighbors = getNeighbors(knightId);
            for (int neighbor : neighbors) {
                raisedHand[neighbor] = false;
            }
            
            if (selectedCount >= requiredKnights) {
                selectionFinished = true;
                break;
            }
        }
        
        if (selectRandomAvailableKnight() == -1) {
            std::fill(raisedHand.begin(), raisedHand.end(), false);
        }
    }
    
    selectionFinished = true;
    
    for (auto& knight : knights) {
        if (knight.joinable()) {
            knight.join();
        }
    }
    
    std::cout << "Selection completed" << std::endl;
}

void KnightSelection::printSelectedKnights() const {
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
