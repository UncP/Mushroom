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

TEST(PageMoveWithNoPrefixWithEmptySlot)
{
	srand(time(0));
	const uint8_t key_len = 10;
	Page *left = Page::NewPage(key_len);
	Page *right = Page::NewPage(key_len);

	char buf[sizeof(page_t) + key_len] = {0};
	KeySlice *key = (KeySlice *)buf;
	string str(key_len, '8');
	for (;;) {
		if (left->Full()) {
			str = string(key_len, '9');
			memcpy(key->key_, str.c_str(), key_len);
			char buf[sizeof(page_t) + key_len] = {0};
			KeySlice *tmp = (KeySlice *)buf;
			memset(tmp->key_, 0xFF, key_len);
			ASSERT_TRUE(left->Update(tmp, key));
			break;
		} else {
			str[rand()%key_len] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}
	for (int i = 0; i < 9; ++i) {
		memcpy(key->key_, str.c_str(), key_len);
		right->Insert(key);
		if (i == 0) str = string(key_len, 'z');
		str[rand()%key_len] -= 1 + rand() % 3;
	}

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());

	ASSERT_TRUE(left->Move(right, key));

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());
	printf("%s", key->ToString(key_len).c_str());
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
			ASSERT_TRUE(left->Update(tmp, key));
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
			ASSERT_TRUE(right->Update(tmp, key));
			break;
		} else {
			right->Insert(key);
			str[2] = '8';
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());

	ASSERT_TRUE(left->Move(right, key));

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());
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
			ASSERT_TRUE(left->Update(tmp, key));
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
			ASSERT_TRUE(right->Update(tmp, key));
			break;
		} else {
			right->Insert(key);
			str[2] = '8';
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());

	ASSERT_TRUE(left->Move(right, key));

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());
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
			ASSERT_TRUE(left->Update(tmp, key));
			break;
		} else {
			str[3 + rand()%(key_len-3)] -= 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
			left->Insert(key);
		}
	}

	ASSERT_FALSE(left->NeedSplit());
	printf("%s\n", left->ToString().c_str());
	left->Expand(2);
	printf("%s\n", left->ToString().c_str());
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
			ASSERT_TRUE(left->Update(tmp, key));
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
			ASSERT_TRUE(right->Update(tmp, key));
			break;
		} else {
			right->Insert(key);
			str[3 + rand()%(key_len-3)] += 1 + rand() % 3;
			memcpy(key->key_, str.c_str(), key_len);
		}
	}

	ASSERT_FALSE(right->NeedSplit());

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());

	ASSERT_TRUE(left->Move(right, key));

	printf("%s\n", left->ToString().c_str());
	printf("%s\n", right->ToString().c_str());
	printf("%s", key->ToString(key_len).c_str());
	Page::DeletePage(left);
	Page::DeletePage(right);
}

int main(int argc, char **argv)
{
	Page::SetPageInfo(256);
	return RUN_ALL_TESTS(argc == 2 ? argv[1] : '\0');
}
