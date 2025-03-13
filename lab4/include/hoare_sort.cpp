#include <vector>

// Отсортировать целочисленный массив;
// Реализация 2 - Сортировка Хоара

extern "C" {
    int* Sort(int* array, int size) {
        if (size < 2) return array;
        int pivot = array[size / 2];
        std::vector<int> left, right;
    
        for (int i = 0; i < size; ++i) {
            if (i == size / 2) continue;
            if (array[i] < pivot) {
                left.push_back(array[i]);
            } else {
                right.push_back(array[i]);
            }
        }
    
        Sort(left.data(), left.size());
        Sort(right.data(), right.size());
    
        std::copy(left.begin(), left.end(), array);
    
        array[left.size()] = pivot;
        std::copy(right.begin(), right.end(), array + left.size() + 1);
        
        return array;
    }
}