/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP
 *    > Description:
 *
 *    > Created Time: 2016-10-14 13:59:25
**/

#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <string>

#include "../src/iterator.hpp"
#include "../src/db.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	std::ifstream in("../data/Str.txt");
	if (!in.is_open()) {
		std::cerr << "文件打开失败 :(\n";
		exit(-1);
	}

	KeySlice::SetStringForm([](const KeySlice *key) {
		// char buf[11];
		// snprintf(buf, 11, "%10d ", *(uint32_t *)key->Data());
		return std::string(key->Data()) + "    ";
	});

	MushroomDB db;
	int key_len = 10;
	db.Open("mushroom", key_len);

	char buf[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	// uint32_t val;
	std::string val;

	int total = argc == 2 ? atoi(argv[1]) : 100;
	for (int i = 0; !in.eof() && i < total; ++i) {
		in >> val;
		// *(uint32_t *)key->Data() = val;
		memcpy(key->Data(), val.c_str(), key_len);
		db.Put(key);
	}
	in.close();
	db.Btree()->Traverse(0);
	std::cout << "done\n";
	// return 0;

	memset(key, 0, sizeof(buf));
	Iterator it(db.Btree());
	it.Begin();
	int count = 0;
	for (; it.Next();) {
		++count;
		if (memcmp(it.Key()->Data(), "9ohb798j6", 10) == 0)
			std::cout << key << it.Key() << std::endl;
		// std::cout << key << it.Key() << std::endl;
		if (Compare(key, it.Key(), key_len) >= 0) {
			std::cout << key << it.Key() << std::endl;
			exit(-1);
		}
		// assert(Compare(key, it.Key(), key_len) < 0);
		memcpy(key, it.Key(), sizeof(buf));
	}
	std::cout << "\ntotal: " << count << std::endl;
	return 0;
}
