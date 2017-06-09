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

	TempSlice(key, key_len);
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

TEST(PageMoveWithNoPrefix)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	TempSlice(key, key_len);
	string str(key_len, '8');
	for (;;) {
		if (left->Full()) {
			str = string(key_len, '9');
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			break;
		} else {
			str[1 + rand()%(key_len-1)] -= (1 + rand() % 3) * ((rand() % 2) ? -1 : 1);
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}
	for (int i = 0; i < 20; ++i) {
		memcpy(key->key_, str.c_str(), key_len);
		right->Insert(key);
		if (i == 0) str = string(key_len, 'z');
		str[rand()%key_len] -= (1 + rand() % 3) * ((rand() % 2) ? -1 : 1);
	}

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	TempSlice(new_key, key_len);
	ASSERT_TRUE(left->Move(right, key, new_key));

	printf("%s\n", left->ToString(false, true).c_str());
	printf("%s\n", right->ToString(false, true).c_str());
	printf("%s", key->ToString(key_len).c_str());
	printf("%s", new_key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
}

TEST(PageMoveWithSamePrefix)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	string str(key_len, '8');
	str[2] = '0';
	for (;;) {
		if (left->Full()) {
			str[2] = '7';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			break;
		} else {
			str[2 + rand()%(key_len-2)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	for (int i = 2; !left->Full();) {
		string tmp = str;
		tmp[i++] -= 3;
		memcpy(key->key_, tmp.c_str(), key_len);
		ASSERT_TRUE(left->Insert(key));
	}

	memcpy(key->key_, str.c_str(), key_len);
	for (;;) {
		if (right->Full()) {
			for (int i = 2; i != key_len; ++i)
				if (str[i] != '8') {
					str[i] = '8';
					break;
				}
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(right->Update(tmp, key, page_no));
			break;
		} else {
			right->Insert(key);
			str[2] = '8';
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	TempSlice(new_key, key_len);
	ASSERT_TRUE(left->Move(right, key, new_key));

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());
	printf("%s", key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
}

TEST(PageMoveWithDifferentPrefix1)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	string str(key_len, '7');
	str[1] = '3';
	str[3] = '0';
	for (;;) {
		if (left->Full()) {
			str[3] = '7';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			break;
		} else {
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	for (int i = 3; !left->Full();) {
		string tmp = str;
		tmp[i++] -= 3;
		memcpy(key->key_, tmp.c_str(), key_len);
		ASSERT_TRUE(left->Insert(key));
	}

	memcpy(key->key_, str.c_str(), key_len);
	for (;;) {
		if (right->Full()) {
			for (int i = 3; i != key_len; ++i)
				if (str[i] != '8') {
					str[i] = '8';
					break;
				}
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(right->Update(tmp, key, page_no));
			break;
		} else {
			right->Insert(key);
			str[2] = '8';
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	TempSlice(new_key, key_len);
	ASSERT_TRUE(left->Move(right, key, new_key));

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());
	printf("%s", key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
}

TEST(PageExpand)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	string str(key_len, '7');
	str[3] = '0';
	for (;;) {
		if (left->Full()) {
			str[3] = '7';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			break;
		} else {
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	printf("%s\n", left->ToString(true, true).c_str());
	left->Expand(2);
	printf("%s\n", left->ToString(true, true).c_str());
	Page::DeletePage(left);
}

TEST(PageMoveWithDifferentPrefix2)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	string str(key_len, '7');
	str[1] = '2';
	str[2] = '0';
	for (;;) {
		if (left->Full()) {
			str[2] = '7';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			break;
		} else {
			str[2 + rand()%(key_len-2)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	for (int i = 3; !left->Full();) {
		string tmp = str;
		tmp[i++] -= 3;
		memcpy(key->key_, tmp.c_str(), key_len);
		ASSERT_TRUE(left->Insert(key));
	}

	memcpy(key->key_, str.c_str(), key_len);
	for (;;) {
		if (right->Full()) {
			str[3] = '9';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(right->Update(tmp, key, page_no));
			break;
		} else {
			right->Insert(key);
			str[3 + rand()%(key_len-3)] += 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	TempSlice(new_key, key_len);
	ASSERT_TRUE(left->Move(right, key, new_key));

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());
	printf("%s", key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
}

TEST(PageCombineWithNoPrefix)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	left->SetPageNo(1);
	right->SetPageNo(2);

	TempSlice(key, key_len);
	string str(key_len, '8');
	for (;;) {
		if (left->Full()) {
			str = string(key_len, '9');
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			break;
		} else {
			str[1+rand()%(key_len-1)] -= (1 + rand() % 3) * ((rand() % 2) ? 1 : -1);
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}
	for (int i = 0; i < 10; ++i) {
		memcpy(key->key_, str.c_str(), key_len);
		right->Insert(key);
		if (i == 0) str = string(key_len, 'z');
		str[1 + rand()%(key_len-1)] -= (1 + rand() % 3) * ((rand() % 2) ? 1 : -1);
	}

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	Page *mid = Page::NewPage(key_len);
	mid->SetPageNo(3);
	TempSlice(old_key, key_len);
	TempSlice(new_key, key_len);
	mid->Combine(left, right, old_key, new_key, key);

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", mid->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());
	printf("%u %s", old_key->page_no_, old_key->ToString(key_len).c_str());
	printf("%u %s", new_key->page_no_, new_key->ToString(key_len).c_str());
	printf("%u %s", key->page_no_, key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
	Page::DeletePage(mid);
}

TEST(PageCombineWithSamePrefix)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);
	left->SetPageNo(1);
	right->SetPageNo(2);

	TempSlice(key, key_len);
	string str(key_len, '7');
	for (;;) {
		if (left->Full()) {
			str[1] = '8';
			key->page_no_ = right->PageNo();
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			key->page_no_ = 0;
			break;
		} else {
			str[2 + rand()%(key_len-2)] -= (1 + rand() % 3) * ((rand() % 2) ? 1 : -1);
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	for (int i = 2; !left->Full();) {
		string tmp = str;
		tmp[i++] -= 3;
		memcpy(key->key_, tmp.c_str(), key_len);
		ASSERT_TRUE(left->Insert(key));
	}

	memcpy(key->key_, str.c_str(), key_len);
	for (;;) {
		if (right->Full()) {
			str[2] = '9';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(right->Update(tmp, key, page_no));
			break;
		} else {
			str[3 + rand()%(key_len-3)] -= (1 + rand() % 3) * ((rand() % 2) ? 1 : -1);
			memcpy(key->key_, str.c_str(), key_len);
			str[1] = '9';
			right->Insert(key);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	Page *mid = Page::NewPage(key_len);
	mid->SetPageNo(3);
	TempSlice(old_key, key_len);
	TempSlice(new_key, key_len);
	mid->Combine(left, right, old_key, new_key, key);

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", mid->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());
	printf("%u %s", old_key->page_no_, old_key->ToString(key_len).c_str());
	printf("%u %s", new_key->page_no_, new_key->ToString(key_len).c_str());
	printf("%u %s", key->page_no_, key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
	Page::DeletePage(mid);
}

TEST(PageCombineWithDifferentPrefix)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);
	left->SetPageNo(1);
	right->SetPageNo(2);

	TempSlice(key, key_len);
	string str(key_len, '7');
	for (;;) {
		if (left->Full()) {
			str[2] = '8';
			key->page_no_ = right->PageNo();
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(left->Update(tmp, key, page_no));
			key->page_no_ = 0;
			break;
		} else {
			str[2 + rand()%(key_len-2)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	for (int i = 3; !left->Full();) {
		string tmp = str;
		tmp[i++] -= 10;
		memcpy(key->key_, tmp.c_str(), key_len);
		ASSERT_TRUE(left->Insert(key));
	}

	memcpy(key->key_, str.c_str(), key_len);
	for (;;) {
		if (right->Full()) {
			str[2] = '9';
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			page_t page_no;
			ASSERT_TRUE(right->Update(tmp, key, page_no));
			break;
		} else {
			memcpy(key->key_, str.c_str(), key_len);
			right->Insert(key);
			str[1] = '9';
			str[2 + rand()%(key_len-2)] -= 1 + rand() % 3;
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());

	Page *mid = Page::NewPage(key_len);
	mid->SetPageNo(3);
	TempSlice(old_key, key_len);
	TempSlice(new_key, key_len);
	mid->Combine(left, right, old_key, new_key, key);

	printf("%s\n", left->ToString(true, true).c_str());
	printf("%s\n", mid->ToString(true, true).c_str());
	printf("%s\n", right->ToString(true, true).c_str());
	printf("%u %s", old_key->page_no_, old_key->ToString(key_len).c_str());
	printf("%u %s", new_key->page_no_, new_key->ToString(key_len).c_str());
	printf("%u %s", key->page_no_, key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
	Page::DeletePage(mid);
}

int main(int argc, char **argv)
{
	Page::SetPageInfo(256);
	return RUN_ALL_TESTS(argc == 2 ? argv[1] : '\0');
}
