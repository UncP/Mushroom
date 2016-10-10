/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 18:03:08
**/

#include <iostream>

#include "../src/bt_page.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	BTreePager pager(3);
	BTreePage *page1 = pager.GetPage(0);
	BTreePage *page2 = pager.GetPage(1);
	BTreePage *page3 = pager.GetPage(2);
	BTreePage *page4 = pager.GetPage(3);
	BTreePage *page5 = pager.GetPage(3);
	std::cout << pager;
	pager.Close();
	return 0;
}
