/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 13:50:46
**/

#include <iostream>

#include "../src/thread_pool.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;
	ThreadPool pool;

	pool.Init();

	pool.Close();

	return 0;
}
