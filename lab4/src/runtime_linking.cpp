#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <dlfcn.h>

int (*PrimeCountFunc)(int, int);
int* (*SortFunc)(int*, int);

void* handle_prime;
void* handle_sort;

void load_libs();

int main() {
    load_libs();
    std::cout << "\n[RUNTIME MENU]\nВыберите опцию:\n1) Подсчет кол-ва простых чисел в интервале [А; Б]\n2) Сортировка целочисленного массива\n";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "Введите натуральные числа A и Б:\n";
        int A, B;
        std::cin >> A >> B;
        std::cout << "Результат: " << (*PrimeCountFunc)(A, B) << std::endl;
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
            
            (*SortFunc)(arr, size);
            
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

    dlclose(handle_prime);
    dlclose(handle_sort);
}

void load_libs() {
    handle_prime = dlopen("libs/prime_sieve.so", RTLD_LAZY);
    handle_sort = dlopen("libs/hoare_sort.so", RTLD_LAZY);

    if (!handle_prime) {
        fprintf(stderr, "Ошибка загрузки библиотеки простых чисел: %s\n", dlerror());
        exit(1);
    }
    if (!handle_sort) {
        fprintf(stderr, "Ошибка загрузки библиотеки сортировки: %s\n", dlerror());
        exit(1);
    }

    dlerror();

    PrimeCountFunc = (int (*)(int, int))dlsym(handle_prime, "PrimeCount");
    char* error = dlerror();
    if (error) {
        fprintf(stderr, "Не удалось найти функцию PrimeCount: %s\n", error);
        exit(1);
    }

    SortFunc = (int* (*)(int*, int))dlsym(handle_sort, "Sort");
    error = dlerror();
    if (error) {
        fprintf(stderr, "Не удалось найти функцию Sort: %s\n", error);
        exit(1);
    }
}