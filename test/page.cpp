/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 16:03:55
**/

#include <cassert>
#include <ctime>

#include "unit.h"

#include "../src/mushroom/page.hpp"

using namespace std;
using namespace Mushroom;

TEST(PageInsert)
{
	const uint8_t key_len = 16;
	Page *page = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;

	for (int i = 1; i <= 256; ++i) {
		if (page->Full())
			break;
		if (!(i % 30)) {
			ASSERT_TRUE(!page->Insert(key));
		} else {
			memcpy(key->key_, &i, sizeof(int));
			ASSERT_TRUE(page->Insert(key));
		}
	}
	Page::DeletePage(page);
}

TEST(PageSplit)
{
	srand(time(0));
	const uint8_t key_len = 16;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	string str = "0123456789012345";
	string max = str;
	int total1 = 10;
	for (int i = 0; i < total1; ++i) {
		memcpy(key->key_, str.c_str(), key_len);
		left->Insert(key);
		str[rand()%key_len] = '0' + rand() % 10;
		if (str > max)
			max = str;
	}
	int total2 = 10;
	str = max;
	for (int i = 0; i < total2; ++i) {
		memcpy(key->key_, str.c_str(), key_len);
		right->Insert(key);
		if (i == 0) str = "abcdefghijklmopq";
		str[rand()%key_len] = 'a' + rand() % 26;
	}

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());

	left->Split(right, key, 3);

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());
	printf("%s", key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
}

int main(int argc, char **argv)
{
	Page::SetPageInfo(4096);
	return RUN_ALL_TESTS(argc == 2 ? argv[1] : '\0');
}
