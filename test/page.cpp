/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 16:03:55
**/

#include <cassert>
#include <ctime>

#include "unit.h"
#include "../src/blink/page.hpp"

using namespace std;
using namespace Mushroom;

TEST(PageExpandBloomFilter)
{
	const uint8_t key_len = 16;
	Page *page = Page::NewPage(key_len);

	TempSlice(key);
	int i = 1;
	page_t page_no;
	for (; i <= 98; ++i) {
		memcpy(key->key_, &i, sizeof(int));
		ASSERT_TRUE(!page->Insert(key, page_no));
	}

	printf("%s", page->Status().c_str());
	i = 99;
	memcpy(key->key_, &i, sizeof(int));
	ASSERT_TRUE(!page->Insert(key, page_no));
	ASSERT_FALSE(page->NeedSplit());
	printf("%s", page->Status().c_str());

	Page::DeletePage(page);
}

TEST(PageNotSplit)
{
	const uint8_t key_len = 16;
	Page *page = Page::NewPage(key_len);

	TempSlice(key);
	int i = 1;
	page_t page_no;
	for (; i <= 98; ++i) {
		memcpy(key->key_, &i, sizeof(int));
		ASSERT_TRUE(!page->Insert(key, page_no));
	}

	printf("%s", page->Status().c_str());
	++i;
	memcpy(key->key_, &i, sizeof(int));
	ASSERT_TRUE(!page->Insert(key, page_no));
	ASSERT_FALSE(page->NeedSplit());
	printf("%s", page->Status().c_str());

	for (++i; !page->NeedSplit(); ++i) {
		memcpy(key->key_, &i, sizeof(int));
		page->Insert(key, page_no);
	}

	printf("%s", page->Status().c_str());
	Page::DeletePage(page);
}

TEST(PagePrefixCompactionWithBloomFilter)
{

}

int main()
{
	Page::SetPageInfo(4096);
	return RUN_ALL_TESTS(0);
}
