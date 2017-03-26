/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:   2017-03-16 17:04:27
**/

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <thread>

#include "../src/slice.hpp"
#include "../src/db.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const std::vector<std::string> files = {
		std::string("../data/2500000_0"),
		std::string("../data/2500000_1"),
		std::string("../data/2500000_2"),
		std::string("../data/2500000_3")
		// std::string("../data/25000000_0"),
		// std::string("../data/25000000_1"),
		// std::string("../data/25000000_2"),
		// std::string("../data/25000000_3")
	};
	assert(argc > 4);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint8_t  hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 1024;
	uint8_t  seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;

	const int total = (argc == 6) ? atoi(argv[5]) : 1;
	const int key_len = 16;

	MushroomDB db("../mushroom", key_len, page_size, pool_size, hash_bits, seg_bits);

	auto beg = std::chrono::high_resolution_clock::now();
	std::vector<std::thread> vec1;
	int all = total == 1 ? 1 : total/files.size();
	for (size_t i = 0; i != files.size(); ++i)
		vec1.push_back(std::thread([&, i] {
			char tmp[sizeof(page_id) + key_len] = {0};
			KeySlice *key = (KeySlice *)tmp;
			int fd = open(files[i].c_str(), O_RDONLY);
			assert(fd > 0);
			char buf[8192];
			int curr = 0, ptr = 0, count = 0;
			bool flag = true;
			for (; (ptr = pread(fd, buf, 8192, curr)) > 0 && flag; curr += ptr) {
				while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0') buf[ptr] = '\0';
				if (ptr) buf[ptr++] = '\0';
				else break;
				for (int i = 0; i < ptr;) {
					int j = 0;
					char *tmp = buf + i;
					for (; buf[i] != '\n' && buf[i] != '\0'; ++i, ++j) ;
					tmp[j] = '\0';
					memcpy(key->Data(), tmp, key_len);
					db.Put(key);
					if (++count == all) {
						flag = false;
						break;
					}
					++i;
				}
			}
			close(fd);
		}));
	for (auto &e : vec1)
		e.join();
	auto end = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntotal: " << (all * files.size()) << "\n";
	std::cerr << "put time: " << std::setw(8) << Time << "  s\n";
	bool flag = true;

	// beg = std::chrono::high_resolution_clock::now();
	// bool flag = true;
	// std::vector<std::thread> vec2;
	// for (size_t i = 0; i != files.size(); ++i)
	// 	vec2.push_back(std::thread([&, i] {
	// 		int fd = open(files[i].c_str(), O_RDONLY);
	// 		if (!db.FindSingle(fd, all))
	// 			__sync_bool_compare_and_swap(&flag, true, false);
	// 		close(fd);
	// 	}));
	// for (auto &e : vec2)
	// 	e.join();
	// end = std::chrono::high_resolution_clock::now();
	// Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	// std::cerr << "get time: " << std::setw(8) << Time << "  s\n";

	db.Close();

	if (!flag)
		std::cerr << "\033[31mFail :(\033[0m\n";
	else
		std::cerr << "\033[32mSuccess :)\033[0m\n";

	return 0;
}
