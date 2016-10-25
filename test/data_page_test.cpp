/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 22:37:44
**/

#include <iostream>

#include "../src/data_page.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;
	std::cout << sizeof(DataPage) << std::endl;
	DataPage *page = nullptr;
	std::cout << ((char *)page->data_ - (char *)page) << std::endl;
	std::cout << ((char *)(&page->curr_) - (char *)page) << std::endl;
	return 0;
}
