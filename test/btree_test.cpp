/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 16:34:08
**/

#include <iostream>

#include "../src/btree.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;
	BTree btree;
	btree.Init(sizeof(uint32_t));
	for (int i = 4; i <= 16; ++i)
		btree.Init(1, i);
	return 0;
}
