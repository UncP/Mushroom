/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-27 17:43:00
**/

#include <pthread.h>

#include "../src/palm/barrier.hpp"

static const int threads = 5;
static Mushroom::Barrier barrier(threads);

void* f1(void *)
{
	printf("hello\n");
	barrier.Wait();
	return (void *)0;
}

void* f2(void *)
{
	printf("world\n");
	barrier.Wait();
	return (void *)0;
}

int main()
{
	pthread_t ids[threads];
	for (int i = 0; i < threads; ++i) {
		assert(pthread_create(&ids[i], 0, f1, 0) == 0);
	}
	for (int i = 0; i != threads; ++i)
		assert(pthread_join(ids[i], 0) == 0);

	for (int i = 0; i < threads; ++i) {
		assert(pthread_create(&ids[i], 0, f2, 0) == 0);
	}
	for (int i = 0; i != threads; ++i)
		assert(pthread_join(ids[i], 0) == 0);
	return 0;
}