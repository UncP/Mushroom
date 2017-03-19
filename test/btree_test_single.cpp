/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-20 12:37:41
**/

#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include <iomanip>

#include "../src/db.hpp"
#include "../src/iterator.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const int total = (argc == 2) ? atoi(argv[1]) : 1;
	const int key_len = 16;
	const char *file = "../data/16_10000000_random";

	MushroomDB db("mushroom", key_len);

	KeySlice::SetStringFormat([](const KeySlice *key, uint8_t len) {
		return std::string(key->Data(), len) + "\n";
	});

	auto beg = std::chrono::high_resolution_clock::now();
	db.IndexSingle(file, total);
	auto end = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntotal: " << total << "\n";
	std::cerr << "put time: " << std::setw(8) << Time << "  s\n";

	beg = std::chrono::high_resolution_clock::now();
	auto flag = db.FindSingle(file, total);
	end = std::chrono::high_resolution_clock::now();
	Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "get time: " << std::setw(8) << Time << "  s\n";
	if (!flag) {
		std::cout << "\033[31mFail :(\033[0m\n";
	} else {
		Iterator it(db.Btree());
		assert(it.CheckBtree());
		std::cout << "\033[32mSuccess :)\033[0m\n";
	}

	db.Close();
	return 0;
}
