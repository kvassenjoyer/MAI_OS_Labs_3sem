# Лабораторная работа №4

## Условие:
Создание и использование динамических библиотек
Вариант 21:
1) Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные):
- Наивный алгоритм - проверить делимость текущего числа на все предыдущие числа.
- Решето Эратосфена.
2)  Отсортировать целочисленный массив:
- Пузырьковая сортировка.
- Сортировка Хоара.
  
## Сборка лабы:

Через скрипт build.sh:
```
#!/bin/bash

mkdir -p ./libs/

g++ -shared -fPIC include/bubble_sort.cpp -o libs/bubble_sort.so
g++ -shared -fPIC include/hoare_sort.cpp -o libs/hoare_sort.so
g++ -shared -fPIC include/prime_naive.cpp -o libs/prime_naive.so
g++ -shared -fPIC include/prime_sieve.cpp -o libs/prime_sieve.so

g++ src/static_linking.cpp -o static
g++ src/runtime_linking.cpp -o runtime

```

## Запуск лабы:
С заготовленным тестом через скрипт run.sh:
```
#!/bin/bash

prime_input="1 1 5000"

sort_input="2 11 52 128 -39 17 49 2672 -2 37 28 192 0"

echo "$prime_input" | ./static
echo "$sort_input" | ./static

echo "$prime_input" | ./runtime
echo "$sort_input" | ./runtime
```
С пользовательским ручным вводом:

```
./static
```
или 
```
./runtime
```


## Отладка лабы с помощью strace:
```
strace -f ./static
strace -f ./runtime
```