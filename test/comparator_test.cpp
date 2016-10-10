/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-08 20:36:12
**/

#include <iostream>
#include <cstring>
#include <cstdint>

inline int compare(const void *a, const void *b)
{
	uint32_t aa = *(uint32_t *)a;
	uint32_t bb = *(uint32_t *)b;
	if (aa > bb) return  1;
	if (aa < bb) return -1;
	return 0;
}

int main(int argc, char **argv)
{
	// std::cout << strncmp("123", "234", 3) << std::endl;
	// std::cout << memcmp("123", "234", 3) << std::endl;

	// std::cout << strncmp("1234567", "1234566", 7) << std::endl;
	// std::cout << memcmp("1234567", "1234566", 7) << std::endl;

	int a = 1234567, b = 1234568;

	// std::cout << memcmp(&a, &b, 4) << std::endl;
	// for (int i = 0; i != 100000000; ++i)
		// memcmp(&a, &b, 4);
	for (int i = 0; i != 100000000; ++i)
		compare(&a, &b);
	return 0;
}
