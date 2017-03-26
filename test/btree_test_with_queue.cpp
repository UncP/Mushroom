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
#include <fcntl.h>
#include <unistd.h>

#include "../src/slice.hpp"
#include "../src/db.hpp"
#include "../src/thread_pool.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const char *file = "../data/10000000";

	assert(argc > 4);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint8_t  hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 1024;
	uint8_t  seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;

	const int total = (argc == 6) ? atoi(argv[5]) : 1;
	const int key_len = 16;

	MushroomDB db("../mushroom", key_len, page_size, pool_size, hash_bits, seg_bits);

	ThreadPool *pool = new ThreadPool(new Queue(1024, key_len));

	auto beg = std::chrono::high_resolution_clock::now();

	char tmp[sizeof(page_id) + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(file, O_RDONLY);
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
			pool->AddTask(&MushroomDB::Put, &db, key);
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	pool->Clear();
	flag = true;
	auto end = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntotal: " << total << "\n";
	std::cerr << "put time: " << std::setw(8) << Time << "  s\n";

	// beg = std::chrono::high_resolution_clock::now();
	// flag = db.FindSingle(file, total);
	// end = std::chrono::high_resolution_clock::now();
	// Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	// std::cerr << "get time: " << std::setw(8) << Time << "  s\n";

	if (!flag)
		std::cerr << "\033[31mFail :(\033[0m\n";
	else
		std::cerr << "\033[32mSuccess :)\033[0m\n";

	db.Close();

	delete pool;
	return 0;
}
