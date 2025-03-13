#include <cmath>
#include <iostream>

// Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные);
// Реализация 1 - Наивный алгоритм. Проверить делимость текущего числа на все предыдущие числа.

extern "C" {
    bool isPrime(int n) {
        if (n <= 1) return false;
        if (n == 2) return true; 
        if (n % 2 == 0) return false; 
    
        for (int i = 3; i <= std::sqrt(n); i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
    
    int PrimeCount(int A, int B) {
        if (B < 2) return 0;
        if (A < 2) A = 2;
    
        int count = 0;
        for (int num = A; num <= B; num++) {
            if (isPrime(num)) {
                ++count;
            }
        }
        return count;
    }
}