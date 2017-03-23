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

#include "../src/db.hpp"
#include "../src/thread_pool.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const int total = (argc == 2) ? atoi(argv[1]) : 1;
	const int key_len = 16;
	const char *file = "../data/16_10000000_random";

	MushroomDB db("../mushroom", key_len);

	KeySlice::SetStringFormat([](const KeySlice *key, uint8_t len) {
		return std::string(key->Data(), len) + "\n";
	});

	ThreadPool *pool = new ThreadPool(new Queue(1024, key_len));

	auto beg = std::chrono::high_resolution_clock::now();

	char tmp[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(file, O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;

	BTree *btree = db.Btree();
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
			pool->AddTask(&BTree::Put, btree, key);
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	pool->Clear();
	auto end = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntotal: " << total << "\n";
	std::cerr << "put time: " << std::setw(8) << Time << "  s\n";

	beg = std::chrono::high_resolution_clock::now();
	flag = db.FindSingle(file, total);
	end = std::chrono::high_resolution_clock::now();
	Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "get time: " << std::setw(8) << Time << "  s\n";
	if (!flag)
		std::cerr << "\033[31mFail :(\033[0m\n";
	else
		std::cerr << "\033[32mSuccess :)\033[0m\n";

	db.Close();

	delete pool;
	return 0;
}
