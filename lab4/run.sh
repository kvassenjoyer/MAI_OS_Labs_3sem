#!/bin/bash

prime_input="1 1 5000"

sort_input="2 11 52 128 -39 17 49 2672 -2 37 28 192 0"

echo "$prime_input" | ./static
echo "$sort_input" | ./static

echo "$prime_input" | ./runtime
echo "$sort_input" | ./runtime
