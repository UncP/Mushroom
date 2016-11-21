/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 13:50:46
**/

#include <iostream>
#include <fstream>

#include "../src/db.hpp"
#include "../src/iterator.hpp"
#include "../src/thread_pool.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;
	const char *file = "../data/Str.txt";
	const int key_len = 10;
	const int total = (argc == 2) ? atoi(argv[1]) : 10;

	MushroomDB db("mushroom", key_len);

	KeySlice::SetStringFormat([](const KeySlice *key) {
		return std::string(key->Data()) + "    ";
	});

	std::ifstream in;
	in.open(file);
	assert(in.is_open());
	std::string val;

	ThreadPool pool(new FiniteQueue<Task>());

	char *buf[total];
	KeySlice *key = nullptr;

	for (int i = 0; !in.eof() && i < total; ++i) {
		buf[i] = new char[BTreePage::PageByte + key_len];
		in >> val;
		key = (KeySlice *)buf[i];
		memcpy(key->Data(), val.c_str(), key_len);
		pool.AddTask(Task(&BTree::Put, (BTree *)db.Btree(), key));
	}
	pool.Clear();

	Iterator it(db.Btree());
	assert(it.CheckBtree());
	assert(db.Btree()->KeyCheck(in, total));

	in.close();
	for (int i = 0; i != total; ++i)
		delete [] buf[i];

	db.Close();
	return 0;
}
