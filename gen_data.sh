#! /bin/sh

g++ -std=c++11 -O3 test_data.cpp -o test_data -lpthread && ./test_data && rm test_data