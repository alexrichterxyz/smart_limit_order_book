#!/bin/sh
rm -f test/test.out
g++ -O3 -Wall -std=c++17 test/main.cpp -o test/test.out
./test/test.out