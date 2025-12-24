#include "KnightSelection.hpp"
#include <iostream>

int main() {
    try {
        // Создаем объект для выбора рыцарей
        KnightSelection selection(12, 5);
        
        // Запускаем процесс выбора
        selection.startSelection();
        
        // Выводим результат
        selection.printSelectedKnights();
        
        // Проверяем корректность выбора
        if (selection.validateSelection()) {
            std::cout << "\nВыбор корректен! Рыцари готовы к походу!" << std::endl;
            return 0;
        } else {
            std::cout << "\nОбнаружены ошибки в выборе рыцарей!" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}
