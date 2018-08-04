/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-07-29 17:02:00
**/

#include <cassert>

#include "../src/blink/slice.hpp"
#include "../src/palm/batcher.hpp"

int main()
{
	using namespace Mushroom;
	Batcher batcher;
	for (uint32_t i = 0; i < batcher.Capacity(); ++i) {
		std::string s = std::to_string(i);
		s.insert(0, 3 - s.size(), '0');
		s = "batcher_test_" + s;
		TempSlice(key);
		memcpy(key->key_, s.c_str(), KeySlice::KeyLen);
		assert(batcher.InsertKeySlice(key));
	}
	assert(batcher.TotalKey() == batcher.Capacity());
	printf("%s\n", batcher.ToString().c_str());
	return 0;
}