#include "../include/prime_naive.cpp"
#include "../include/bubble_sort.cpp"

#include <iostream>

int main() {
    std::cout << "\n[STATIC MENU]\nВыберите опцию:\n1) Подсчет кол-ва простых чисел в интервале [А; Б]\n2) Сортировка целочисленного массива\n";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "Введите натуральные числа A и Б:\n";
        int A, B;
        std::cin >> A >> B;

        std::cout << "Результат: " << PrimeCount(A, B) << std::endl;
    } else if (choice == 2) {
        std::cout << "Введите длину массива:\n";
        int size;
        std::cin >> size;

        if (size < 0) {
            std::cout << "Некорректная длина массива!\n";
        } 
        else {
            std::cout << "Введите " << size << " целых чисел, элементы массива:\n";
            int* arr = new int[size];
            for (int i = 0; i < size; ++i) {
                std::cin >> arr[i]; 
            }
            
            Sort(arr, size);
            
            std::cout << "Результат: ";
            for (int i = 0; i < size; ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
            delete[] arr;
        }
    } else {
        std::cout << "Некорретный ввод!\n";
    }
}