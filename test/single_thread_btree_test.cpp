/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-28 21:25:06
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

	const char *file = "../data/1000.txt";
	const int key_len = 16;
	const int val_len = 100;
	const int total = (argc == 2) ? atoi(argv[1]) : 10;

	MushroomDB db("mushroom", key_len);

	char buf1[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)buf1;
	char buf2[DataSlice::LengthByte + val_len] = {0};
	DataSlice *val = (DataSlice *)buf2;
	val->SetLength(val_len);
	KeySlice::SetStringFormat([](const KeySlice *key) {
		return std::string(key->Data()) + "    ";
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

	// std::ifstream in(file);
	// assert(in.is_open());
	// if (!db.Btree()->KeyCheck(in, total)) {
	// 	std::cout << "Error :( -----------------\n";
	// } else {
	// 	Iterator it(db.Btree());
	// 	assert(it.CheckBtree());
	// 	in.close();
	// 	std::cout << "!!!!!!!!!!  Success :)  !!!!!!!!!!!!\n";
	// }

	db.Close();
	return 0;
}
