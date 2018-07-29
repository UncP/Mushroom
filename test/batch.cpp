/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-28 15:36:00
**/

#include <cstdio>

#include "../src/blink/slice.hpp"
#include "../src/palm/batch.hpp"

int main()
{
	Mushroom::Batch::SetSize(8);
	Mushroom::Batch batch;

	const char *str = "hello world >:)<";
	batch.SetKeySlice(7, str);
	auto slice = batch.GetKeySlice(7);

	assert(!memcmp(str, slice->key_, Mushroom::KeySlice::KeyLen));
	printf("%s", slice->ToString().c_str());
	return 0;
}