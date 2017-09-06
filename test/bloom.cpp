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

static int NextLength(int length) {
	if (length < 10) {
		length += 1;
	} else if (length < 100) {
		length += 10;
	} else if (length < 1000) {
		length += 100;
	} else {
		length += 1000;
	}
	return length;
}

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

TEST(VaryingLengths_BloomFilter) {
	char buffer[sizeof(int)];

	// Count number of filters that significantly exceed the false positive rate
	int mediocre_filters = 0;
	int good_filters = 0;

	for (int length = 1; length <= 10000; length = NextLength(length)) {
		BloomFilter f(length);
		for (int i = 0; i < length; i++) {
			memcpy(buffer, &i, sizeof(int));
			f.Add(buffer, sizeof(int));
		}

		ASSERT_LE((int)f.size(), ((length * 10 / 8) + 40));

		for (int i = 0; i < length; i++) {
			memcpy(buffer, &i, sizeof(int));
			ASSERT_TRUE(f.Match(buffer, sizeof(int)));
		}

		int result = 0;
		for (int i = 0; i < 10000; i++) {
			int tmp = i + 1000000000;
			memcpy(buffer, &tmp, sizeof(int));
			if (f.Match(buffer, sizeof(int)))
				++result;
		}
		double rate = double(result) / 10000.0;

		printf("false positives: %5.2f%% @ length = %6d ; bytes = %6d\n",
		        rate * 100.0, length, static_cast<int>(f.size()));
		// ASSERT_LE(rate, 0.02);   // Must not be over 2%
		if (rate > 0.0125) mediocre_filters++;  // Allowed, but not too often
		else good_filters++;
	}
	printf("Filters: %d good, %d mediocre\n", good_filters, mediocre_filters);
	ASSERT_LE(mediocre_filters, good_filters / 5);
}

int main()
{
	return RUN_ALL_TESTS(0);
}
