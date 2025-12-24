#include "KnightSelection.hpp"
#include <iostream>

int main() {
    std::cout << "=== Knight Selection Program ===" << std::endl;
    
    try {
        KnightSelection selection(12, 5);
        
        std::cout << "\nStarting selection process..." << std::endl;
        selection.startSelection();
        
        std::cout << "\n=== Selection Results ===" << std::endl;
        selection.printSelectedKnights();
        
        if (selection.validateSelection()) {
            std::cout << "\nSUCCESS: Selection is valid!" << std::endl;
            std::cout << "The knights are ready for their mission." << std::endl;
            return 0;
        } else {
            std::cout << "\nWARNING: Selection has issues" << std::endl;
            std::cout << "But the knights will still go on their mission." << std::endl;
            return 0; // Возвращаем 0 даже с предупреждением
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return 1;
    }
}
