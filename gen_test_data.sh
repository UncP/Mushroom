#! /bin/sh

g++ -std=c++11 -O3 test_data.cpp -o test_data && ./test_data && rm test_data
mv data/10000000_0 data/10000000
