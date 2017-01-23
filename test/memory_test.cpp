/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-01-22 12:54:03
**/

#include <iostream>
#include <cassert>

int main()
{
	char *a = new char[4096];
	char *b = new char[4096];
	char *c = new char[4096];
	char *d = new char[4096];
	printf("%ld %p\n", (uint64_t)a, a);
	printf("%ld %p\n", (uint64_t)b, b);
	assert(b == a + 0x1010);
	printf("%ld %p\n", (uint64_t)c, c);
	assert(c == a + 2 * 0x1010);
	assert(d == a + 3 * 0x1010);
	return 0;
}
