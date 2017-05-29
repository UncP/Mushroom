/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 16:03:55
**/

#include <cassert>

#include "../src/mushroom/page.hpp"

int main()
{
	using namespace Mushroom;
	uint8_t key_len = 16;
	Page::SetPageInfo(4096);
	Page *page = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;

	for (int i = 1; i <= 256; ++i) {
		if (page->Full())
			break;
		if (!(i % 30)) {
			assert(!page->Insert(key));
		} else {
			memcpy(key->key_, &i, sizeof(int));
			assert(page->Insert(key));
		}
	}
	Page::DeletePage(page);
	return 0;
}
