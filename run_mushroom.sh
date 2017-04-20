#! /bin/sh

g++ -std=c++11 -O3 test_data.cpp -o test_data && ./test_data && rm test_data
cd src
make lsmtree_with_queue && ./lsmtree_with_queue 4096 256 6 3 $1