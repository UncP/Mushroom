/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-20 12:37:41
**/

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <iomanip>
#ifndef SingleThread
#include <thread>
#endif

#include "../src/iterator.hpp"
#include "../src/db.hpp"
#include "../src/thread_pool.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	#ifdef Sequential
		const int key_len = 10;
		const char *file = "../data/10000000_sequential";
	#else
		const int key_len = 16;
		const char *file = "../data/16_10000000_random";
	#endif
	const int total = (argc == 2) ? atoi(argv[1]) : 1;

	MushroomDB db("mushroom", key_len);

	KeySlice::SetStringFormat([](const KeySlice *key) {
		return std::string(key->Data()) + "    ";
	});

	char tmp[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(file, O_RDONLY);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;
	#ifndef SingleThread
	bool stop = false;
	std::thread check([&]() {
		using namespace std::chrono_literals;
		while (!stop) {
			std::this_thread::sleep_for(1s);
			std::cout << db.Btree()->inserted_ << std::endl;
			std::cout << db.Btree()->LM()->ToString();
		}
	});
	#endif
	auto beg = std::chrono::high_resolution_clock::now();

	for (; (ptr = pread(fd, buf, 8192, curr)) > 0 && flag; curr += ptr) {
		while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0' && buf[ptr] != ' ') buf[ptr] = '\0';
		if (ptr) buf[ptr++] = '\0';
		else break;
		for (int i = 0; i < ptr;) {
			int j = 0;
			char *tmp = buf + i;
			for (; buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\0'; ++i, ++j) ;
			tmp[j] = '\0';
			memcpy(key->Data(), tmp, key_len);
			db.Put(key);
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	auto end  = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntime: " << std::setw(8) << Time << "  s\n";

	db.ClearTask();

	#ifndef SingleThread
	stop = true;
	check.join();
	#endif

	std::ifstream in(file);
	assert(in.is_open());
	if (!db.Btree()->KeyCheck(in, total)) {
		std::cout << "\033[31mError :(\033[0m\n";
	} else {
		Iterator it(db.Btree());
		assert(it.CheckBtree());
		std::cout << "\033[32mSuccess :)\033[0m\n";
	}
	in.close();

	db.Close();
	return 0;
}
