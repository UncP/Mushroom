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

	const char *file = "../data/16.txt";
	const int key_len = 16;
	const int total = (argc == 2) ? atoi(argv[1]) : 100;

	MushroomDB db("mushroom", key_len);

	char buf[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;

	KeySlice::SetStringFormat([](const KeySlice *key) {
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

	// db.Btree()->Traverse(-1);
	// assert(db.Btree()->KeyCheck(in, total));
	// Iterator it(db.Btree());
	// assert(it.CheckBtree());

	in.close();
	db.Close();
	return 0;
}
