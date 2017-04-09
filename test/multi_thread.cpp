/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:   2017-03-16 17:04:27
**/

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <chrono>
#include <pthread.h>

#include "../src/slice.hpp"
#include "../src/db.hpp"

using namespace Mushroom;

static const int key_len = 16;

static const char *files[] = {
	"../data/2500000_0",
	"../data/2500000_1",
	"../data/2500000_2",
	"../data/2500000_3"
};

struct ThreadArg
{
	ThreadArg() { }
	int i;
	int all;
	MushroomDB *db;
	bool (MushroomDB::*(fun))(KeySlice *);
};

void* Do(void *arg)
{
	int all = ((ThreadArg *)arg)->all;
	MushroomDB *db = ((ThreadArg *)arg)->db;
	bool (MushroomDB::*(fun))(KeySlice *);
	fun = ((ThreadArg *)arg)->fun;

	char tmp[sizeof(page_id) + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(files[((ThreadArg *)arg)->i], O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;
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
			(db->*fun)(key);
			if (++count == all) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	return 0;
}

int main(int argc, char **argv)
{
	assert(argc > 4);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint32_t hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 1024;
	uint32_t seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;

	const int total = (argc == 6) ? atoi(argv[5]) : 1;

	MushroomDB db(key_len, page_size, pool_size, hash_bits, seg_bits);

	int thread_num = 4;
	auto beg = std::chrono::high_resolution_clock::now();
	int all = total == 1 ? 1 : total / thread_num;
	pthread_t ids[thread_num];
	ThreadArg args[thread_num];
	for (int i = 0; i != thread_num; ++i) {
		args[i].i = i;
		args[i].all = all;
		args[i].db = &db;
		args[i].fun = &MushroomDB::Put;
		assert(pthread_create(&ids[i], 0, Do, &args[i]) == 0);
	}
	for (int i = 0; i != thread_num; ++i)
		assert(pthread_join(ids[i], 0) == 0);
	auto end = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	printf("\ntotal: %d\nput time: %f  s\n", all * thread_num, t1);

	// beg = std::chrono::high_resolution_clock::now();
	// for (int i = 0; i != thread_num; ++i) {
	// 	args[i].i = i;
	// 	args[i].all = all;
	// 	args[i].db = &db;
	// 	args[i].fun = &MushroomDB::Get;
	// 	assert(pthread_create(&ids[i], 0, Do, &args[i]) == 0);
	// }
	// for (int i = 0; i != thread_num; ++i)
	// 	assert(pthread_join(ids[i], 0) == 0);
	// end = std::chrono::high_resolution_clock::now();
	// auto t2 = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	// printf("get time: %f  s\n", t2);

	db.Close();

	return 0;
}
