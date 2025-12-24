#ifndef KNIGHT_SELECTION_HPP
#define KNIGHT_SELECTION_HPP

#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <random>
#include <iostream>

class KnightSelection {
public:
    KnightSelection(int totalKnights = 12, int requiredKnights = 5);
    
    void startSelection();
    
    void printSelectedKnights() const;
    
    std::vector<int> getSelectedKnights() const;
    
    bool validateSelection() const;

private:
    const int totalKnights;
    const int requiredKnights;
    std::vector<bool> selected;
    std::vector<bool> handRaised;
    std::atomic<int> selectedCount;
    std::atomic<bool> stopFlag;
    
    mutable std::mutex mtx;
    std::condition_variable cv;
    
    std::random_device rd;
    std::mt19937 gen;
    
    void knightProcess(int id);
    
    bool canRaiseHand(int id) const;
    
    std::vector<int> getNeighbors(int id) const;
};

#endif
