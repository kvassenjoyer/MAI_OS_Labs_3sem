#include <vector>

// Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные);
// Реализация 2 - Решето Эратосфена 

extern "C" {
    int PrimeCount(int A, int B) {
        std::vector<bool> is_prime(B + 1, true);
        is_prime[0] = is_prime[1] = false;
        for (int i = 2; i * i <= B; ++i) {
            if (is_prime[i]) {
                for (int j = i * i; j <= B; j += i) {
                    is_prime[j] = false;
                }
            }
        }
        int count = 0;
        for (int i = A; i <= B; ++i) {
            if (is_prime[i]) {
                ++count;
            }
        }
        return count;
    }  
}
