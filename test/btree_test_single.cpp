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

	auto beg = std::chrono::high_resolution_clock::now();
	db.IndexSingle(file, total);
	auto end  = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntime: " << std::setw(8) << Time << "  s\n";

	if (db.FindSingle(file, total) == Fail) {
		std::cout << "\033[31mError :(\033[0m\n";
	} else {
		Iterator it(db.Btree());
		assert(it.CheckBtree());
		std::cout << "\033[32mSuccess :)\033[0m\n";
	}

	db.Close();

	// KeySlice::SetStringFormat([](const KeySlice *key) {
	// 	return std::string(key->Data()) + "    ";
	// });

	// #ifndef SingleThread
	// bool stop = false;
	// std::thread check([&]() {
	// 	using namespace std::chrono_literals;
	// 	while (!stop) {
	// 		std::this_thread::sleep_for(1s);
	// 		std::cout << db.Btree()->inserted_ << std::endl;
	// 		std::cout << db.Btree()->LM()->ToString();
	// 	}
	// });
	// #endif

	// #ifndef SingleThread
	// stop = true;
	// check.join();
	// #endif
	return 0;
}
