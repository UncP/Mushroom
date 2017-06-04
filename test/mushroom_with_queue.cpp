/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-11-20 12:37:41
**/

#include <cassert>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>

#include "../src/mushroom/slice.hpp"
#include "../src/mushroom/db.hpp"
#include "../src/mushroom/task.hpp"
#include "../src/include/bounded_mapping_queue.hpp"
#include "../src/include/thread_pool_mapping.hpp"

using namespace Mushroom;

static const int key_len = 16;
static int total;

double Do(const char *file, MushroomDB *db, bool (MushroomDB::*(fun))(KeySlice *))
{
	BoundedMappingQueue<MushroomTask> *queue = new BoundedMappingQueue<MushroomTask>(1024, []() {
		return new MushroomTask(key_len);
	});

	ThreadPoolMapping<MushroomTask> pool(queue, 4);

	TempSlice(key, sizeof(valptr) + key_len);
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
			memcpy(key->key_, tmp, key_len);

			MushroomTask *task = queue->Get();
			task->Assign(fun, db, key);
			queue->Push();

			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);

	pool.Clear();
	delete queue;

	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	return t;
}

int main(int argc, char **argv)
{
	const char *file = "../data/10000000";

	assert(argc > 5);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint32_t hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 10;
	uint32_t seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;
	uint32_t log_page  = atoi(argv[5]) ? atoi(argv[5]) : 16;

	total = (argc == 7) ? atoi(argv[6]) : 1;

	MushroomDB db("mushroom_test", key_len, page_size, pool_size, hash_bits, seg_bits, log_page);

	double t1 = Do(file, &db, &MushroomDB::Put);

	double t2 = Do(file, &db, &MushroomDB::Get);

	db.Close();

	printf("\033[31mtotal: %d\033[0m\n\033[32mput time: %f  s\033[0m\n", total, t1);
	printf("\033[34mget time: %f  s\033[0m\n", t2);

	return 0;
}
