/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-22 18:39:17
**/

#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>

#include "../src/db.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const int total = (argc == 2) ? atoi(argv[1]) : 1;
	const int key_len = 16;
	const std::vector<std::string> files = {
		std::string("../data/16_2500000_0_random"),
		std::string("../data/16_2500000_1_random"),
		std::string("../data/16_2500000_2_random"),
		std::string("../data/16_2500000_3_random")
	};

	KeySlice::SetStringFormat([](const KeySlice *key, uint8_t len) {
		return std::string(key->Data(), len) + "\n";
	});

	MushroomDB db("../mushroom", key_len);

	pid_t id = fork();
	if (!id) {
		// auto beg = std::chrono::high_resolution_clock::now();
		db.IndexSingle(files[0].c_str(), total);
		// auto end = std::chrono::high_resolution_clock::now();
		// auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
		std::cout << "\ntotal: " << total << "\n";
		// std::cout << "put time: " << std::setw(8) << Time << "  s\n";
	} else {
		int status;
		waitpid(id, &status, 0);
		auto beg = std::chrono::high_resolution_clock::now();
		bool flag = db.FindSingle(files[0].c_str(), total);
		auto end = std::chrono::high_resolution_clock::now();
		auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
		std::cout << "get time: " << std::setw(8) << Time << "  s\n";
		if (!flag) {
			std::cout << "\033[31mFail :(\033[0m\n";
		} else {
			std::cout << "\033[32mSuccess :)\033[0m\n";
		}
	}
	db.Close();
	return 0;
}
