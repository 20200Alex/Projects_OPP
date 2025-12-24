#ifndef KNIGHT_SELECTION_HPP
#define KNIGHT_SELECTION_HPP

#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <random>

/**
 * @class KnightSelection
 * Реализует логику выбора 5 рыцарей так, чтобы соседи не выбирались вместе
 */
class KnightSelection {
public:
    /**
     * @brief Конструктор класса
     * @param totalKnights Общее количество рыцарей
     * @param requiredKnights Требуемое количество рыцарей для выбора
     */
    KnightSelection(int totalKnights = 12, int requiredKnights = 5);
    /**
     * @brief Запускает процесс выбора рыцарей
     */
    void startSelection();
    /**
     * @brief Выводит список выбранных рыцарей
     */
    void printSelectedKnights() const;
    /**
     * @brief Возвращает список ID выбранных рыцарей
     * @return Вектор с ID выбранных рыцарей
     */
    std::vector<int> getSelectedKnights() const;
    /**
     * @brief Проверяет корректность выбора
     * @return true если выбор корректен, false в противном случае
     */
    bool validateSelection() const;

private:
    const int totalKnights;              // Общее количество рыцарей
    const int requiredKnights;           // Требуемое количество для выбора
    std::vector<bool> selected;          // Флаги выбранных рыцарей
    std::vector<bool> raisedHand;        // Флаги поднятых рук
    std::atomic<int> selectedCount;      // Счетчик выбранных рыцарей
    std::atomic<bool> selectionFinished; // Флаг завершения выбора
    
    mutable std::mutex mtx;              // Мьютекс для синхронизации
    std::condition_variable cv;          // Условная переменная для ожидания
    
    std::random_device rd;               // Генератор случайных чисел
    std::mt19937 gen;                    // Вихрь Мерсенна для случайности
    /**
     * @brief Потоковая функция для рыцаря
     * @param id ID рыцаря (0-based)
     */
    void knightThread(int id);
    /**
     * @brief Проверяет, может ли рыцарь поднять руку
     * @param id ID рыцаря
     * @return true если может поднять руку, false в противном случае
     */
    bool canRaiseHand(int id) const;
    /**
     * @brief Возвращает соседей рыцаря
     * @param id ID рыцаря
     * @return Вектор с ID соседей
     */
    std::vector<int> getNeighbors(int id) const;
    /**
     * @brief Выбирает случайного рыцаря из доступных
     * @return ID выбранного рыцаря или -1 если нет доступных
     */
    int selectRandomAvailableKnight();
};

#endif // KNIGHT_SELECTION_HPP
