/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-18 14:17:42
**/

#include <iostream>

#include "../src/task.hpp"
#include "../src/queue.hpp"
#include "../src/btree.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	BTree btree;
	assert(btree.Init(3, 10));
	Queue<Task> queue;

	char buf[256];
	KeySlice *key = (KeySlice *)buf;

	key->Assign(0, "12345678", 8);
	Task task1(&BTree::Put, &btree, key);
	queue.Push(task1);
	task1 = queue.Pop();
	task1();
	assert(btree.Get(key));

	key->Assign(0, "23456789", 8);
	queue.Push(task1);

	task1 = queue.Pop();
	task1();
	assert(btree.Get(key));

	queue.Clear();
	return 0;
}
