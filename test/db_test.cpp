/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-14 13:59:25
**/

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include "../src/iterator.hpp"
#include "../src/db.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const char *file = "../data/Str.txt";
	const int key_len = 10;
	const int total = (argc == 2) ? atoi(argv[1]) : 1000000;

	MushroomDB db;
	assert(db.Open("mushroom", key_len));

	char buf[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;

	KeySlice::SetStringForm([](const KeySlice *key) {
		return std::string(key->Data()) + "    ";
	});

	std::ifstream in;
	in.open(file);
	assert(in.is_open());
	std::string val;
	for (int i = 0; !in.eof() && i < total; ++i) {
		in >> val;
		memcpy(key->Data(), val.c_str(), key_len);
		db.Put(key);
	}
	in.seekg(0);
	for (int i = 0; !in.eof() && i != total; ++i) {
		in >> val;
		memcpy(key->Data(), val.c_str(), key_len);
		assert(db.Get(key));
	}
	in.close();

	int count = 0, level = 0;
	for (;; ++level) {
		Iterator it(db.Btree(), level);
		if (it.Begin()) {
			count = 0;
			memset(key, 0, sizeof(buf));
			for (; it.Next();) {
				++count;
				assert(Compare(key, it.Key(), key_len) < 0);
				memcpy(key, it.Key(), sizeof(buf));
			}
		} else {
			break;
		}
		std::cout << "\nlevel: " << level << "  total: " << count << std::endl;
	}
	db.Close();
	return 0;
}
