/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-14 15:45:13
**/

#include "unit.h"
#include "../src/raft/mushroom_log_vector.hpp"

using namespace Mushroom;

TEST(Append)
{
	MushroomLogVector vec;
	MushroomLog *log = NewMushroomLog();
	log->term_ = 1234;
	const char *str = "hello world ! :)";
	log->key_->page_no_ = 4321;
	memcpy(log->key_->key_, str, 16);
	int total = 10000;
	for (int i = 0; i < total; ++i)
		vec.Append(*log);
	DeleteMushroomLog(log);
	ASSERT_TRUE(int(vec.size()) == total);
}

TEST(Get)
{
	MushroomLogVector vec;
	MushroomLog *log = NewMushroomLog();
	log->term_ = 1234;
	const char *str = "hello world ! :)";
	log->key_->page_no_ = 43210;
	memcpy(log->key_->key_, str, 16);
	int total = 100;
	for (int i = 0; i < total; ++i)
		vec.Append(*log);
	for (int i = 0; i < total; ++i) {
		MushroomLog &l = vec[total - 1];
		ASSERT_TRUE(l.term_ == log->term_);
		ASSERT_TRUE(!memcmp(&l, log, KeySlice::KeySize));
	}
	DeleteMushroomLog(log);
}

int main()
{
	return RUN_ALL_TESTS('\0');
}
