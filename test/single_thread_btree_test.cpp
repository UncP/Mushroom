/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-28 21:25:06
**/

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <iomanip>

#include "../src/iterator.hpp"
#include "../src/db.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const char *file = "../data/1000000";
	const int key_len = 10;
	const int total = (argc == 2) ? atoi(argv[1]) : 1;

	MushroomDB db("mushroom", key_len);

	char tmp[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	KeySlice::SetStringFormat([](const KeySlice *key) {
		size_t len = 13 - strlen(key->Data());
		std::string tail;
		for (size_t i = 0; i < len; ++i)
			tail += ' ';
		return std::string(key->Data()) + tail;
	});

	int fd = open(file, O_RDONLY);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;
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
	auto end  = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntime: " << std::setw(8) << Time << "  s\n";
	close(fd);

	std::ifstream in(file);
	assert(in.is_open());
	if (!db.Btree()->KeyCheck(in, total)) {
		std::cout << "\033[31mError :(\033[0m\n";
	} else {
		Iterator it(db.Btree());
		assert(it.CheckBtree());
		in.close();
		std::cout << "\033[32mSuccess :)\033[0m\n";
	}

	db.Close();
	return 0;
}
