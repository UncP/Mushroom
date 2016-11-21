/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP
 *    > Description:
 *
 *    > Created Time: 2016-11-20 12:37:41
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
#include "../src/thread_pool.hpp"

char *keys[10000000];

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const char *file = "../data/1000.txt";
	const int key_len = 16;
	const int total = (argc == 2) ? atoi(argv[1]) : 10;

	MushroomDB db("mushroom", key_len);
	ThreadPool pool(new FiniteQueue<Task>());

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
			keys[count] = new char[BTreePage::PageByte + key_len];
			assert(keys[count]);
			KeySlice *key = (KeySlice *)keys[count];
			memcpy(key->Data(), tmp, key_len);
			pool.AddTask(Task(&BTree::Put, (BTree *)db.Btree(), key));
			if (++count == total) {
				flag = false;
				break;
			} else if (count == 1000000) {
				std::cout << "1000000\n";
			}
			++i;
		}
	}
	pool.Clear();
	close(fd);
	auto end  = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntime: " << std::setw(8) << Time << "  s\n";

	std::ifstream in(file);
	assert(in.is_open());
	if (!db.Btree()->KeyCheck(in, total)) {
		// db.Btree()->Traverse(-1);
		// db.Btree()->Traverse(0);
		std::cout << "Error :( -----------------\n";
	} else {
		Iterator it(db.Btree());
		assert(it.CheckBtree());
		in.close();
		std::cout << "!!!!!!!!!!  Success :)  !!!!!!!!!!!!\n";
	}

	for (int i = 0; i != total; ++i)
		delete [] keys[i];

	db.Close();
	return 0;
}
