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

#include "../src/iterator.hpp"
#include "../src/db.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	std::ifstream in("../data/Int.txt");
	if (!in.is_open()) {
		std::cerr << "文件打开失败 :(\n";
		exit(-1);
	}

	KeySlice::SetOutput([](const KeySlice *key) {
		std::cout << std::setw(10) << *(uint32_t *)key->Data();
	});

	MushroomDB db;
	db.Open("mushroom", sizeof(uint32_t));

	char buf[BTreePage::PageByte + sizeof(uint32_t)];
	KeySlice *key = (KeySlice *)buf;
	memset(buf, 0, BTreePage::PageByte);
	uint32_t val;

	for (; !in.eof(); ) {
		in >> val;
		*(uint32_t *)key->Data() = val;
		db.Put(key);
	}
	in.close();

	std::cout << db.Btree();

	memset(key, 0, sizeof(key));
	Iterator it(db.Btree());
	it.Begin();
	for (; it.Next();) {
		std::cout << it.Key();
		assert(Compare(key, it.Key(), 4) < 0);
		memcpy(key, it.Key(), sizeof(key));
	}
	return 0;
}
