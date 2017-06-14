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
	memcpy(log->key_->key_, str, 16);
	vec.Append(*log);
	DeleteMushroomLog(log);
	ASSERT_TRUE(vec.size() == 1);
}

int main()
{
	return RUN_ALL_TESTS('\0');
}