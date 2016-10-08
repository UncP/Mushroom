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

int main(int argc, char **argv)
{
	std::cout << strncmp("123", "234", 3) << std::endl;
	std::cout << memcmp("123", "234", 3) << std::endl;

	std::cout << strncmp("2356", "a359", 4) << std::endl;
	std::cout << memcmp("2356", "a359", 4) << std::endl;
	return 0;
}
