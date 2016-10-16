/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 15:52:56
**/

#include <iostream>

#include "../src/slice.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	int a[] = {12345, 0, 3};
	Slice *slice = static_cast<Slice *>(static_cast<void *>(a));
	// Slice slice("slice test :)");
	// std::cout << slice;
	std::cout << (int)slice->a << std::endl;
	std::cout << (int)slice->data[0] << std::endl;

	return 0;
}
