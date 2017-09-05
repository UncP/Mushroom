/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-09-05 22:48:34
**/

#include <string>

#include "unit.h"
#include "../src/blink/bloom.hpp"

using namespace Mushroom;

TEST(Empty_BloomFilter)
{
	BloomFilter f(0);
	ASSERT_FALSE(f.Match("hello", 5));
	ASSERT_FALSE(f.Match("world", 5));
}

TEST(Simple_BloomFilter)
{
	BloomFilter f(2);
	ASSERT_FALSE(f.Match("hello", 5));
	ASSERT_FALSE(f.Match("world", 5));
	f.Add("hello", 5);
	f.Add("world", 5);
	ASSERT_TRUE(f.Match("hello", 5));
	ASSERT_TRUE(f.Match("world", 5));
}

int main()
{
	return RUN_ALL_TESTS(0);
}