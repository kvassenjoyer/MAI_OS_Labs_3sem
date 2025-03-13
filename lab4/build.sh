#!/bin/bash

mkdir -p ./libs/

g++ -shared -fPIC include/bubble_sort.cpp -o libs/bubble_sort.so
g++ -shared -fPIC include/hoare_sort.cpp -o libs/hoare_sort.so
g++ -shared -fPIC include/prime_naive.cpp -o libs/prime_naive.so
g++ -shared -fPIC include/prime_sieve.cpp -o libs/prime_sieve.so

g++ src/static_linking.cpp -o static
g++ src/runtime_linking.cpp -o runtime
