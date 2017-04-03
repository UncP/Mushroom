/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-20 12:37:41
**/

#include <cassert>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>

#include "../src/slice.hpp"
#include "../src/db.hpp"

#ifndef NOLATCH
#include "../src/thread_pool.hpp"
#endif

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const char *file = "../data/10000000";

	assert(argc > 4);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint8_t  hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 1024;
	uint8_t  seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;

	const int total = (argc == 6) ? atoi(argv[5]) : 1;
	const int key_len = 16;

	MushroomDB db(key_len, page_size, pool_size, hash_bits, seg_bits);

	#ifndef NOLATCH
	ThreadPool *pool = new ThreadPool(new Queue(1024, key_len));
	#else
	printf("NOLATCH defined, using single thread ;)\n");
	#endif
	char tmp[sizeof(page_id) + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(file, O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;

	auto beg = std::chrono::high_resolution_clock::now();
	for (; (ptr = pread(fd, buf, 8192, curr)) > 0 && flag; curr += ptr) {
		while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0') buf[ptr] = '\0';
		if (ptr) buf[ptr++] = '\0';
		else break;
		for (int i = 0; i < ptr;) {
			int j = 0;
			char *tmp = buf + i;
			for (; buf[i] != '\n' && buf[i] != '\0'; ++i, ++j) ;
			tmp[j] = '\0';
			memcpy(key->Data(), tmp, key_len);
			#ifndef NOLATCH
			pool->AddTask(&MushroomDB::Put, &db, key);
			#else
			db.Put(key);
			#endif
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	#ifndef NOLATCH
	pool->Clear();
	#endif
	flag = true;
	auto end = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	printf("\ntotal: %d\nput time: %f  s\n", total, Time);

	// beg = std::chrono::high_resolution_clock::now();
	// fd = open(file, O_RDONLY);
	// flag = db.FindSingle(fd, total);
	// close(fd);
	// end = std::chrono::high_resolution_clock::now();
	// Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	// printf("get time: %f  s\n", Time);

	if (!flag)
		printf("\033[31mFail :(\033[0m\n");
	else
		printf("\033[32mSuccess :)\033[0m\n");

	db.Close();

	#ifndef NOLATCH
	delete pool;
	#endif
	return 0;
}
