/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:34:43
**/

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fstream>
#include <thread>
#include <iostream>

#include "db.hpp"
#include "page_manager.hpp"
#include "latch_manager.hpp"
#include "thread_pool.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, const int key_len)
{
	if (!access(name, F_OK)) assert(!remove(name));

	assert((sizeof(LatchManager) + 2 * sizeof(page_id)) ==
		BTreePage::PageSize * PageManager::LatchPages);

	assert((fd_ = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) > 0);

	struct flock lock;
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_len = 2 * sizeof(page_id);
	assert(fcntl(fd_, F_SETLKW, &lock) != -1);

	if (!lseek(fd_, 0, SEEK_END)) {
		page_id page_no[2] = {0, 0};
		assert(write(fd_, (void *)page_no, 2 * sizeof(page_id)) == 2 * sizeof(page_id));
		LatchManager tmp;
		assert(write(fd_, (void *)&tmp, sizeof(LatchManager)) == sizeof(LatchManager));
	}
	lock.l_type = F_UNLCK;
	assert(fcntl(fd_, F_SETLKW, &lock) != -1);

	mapped_ = (char *)mmap(0, BTreePage::PageSize * PageManager::LatchPages,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	assert(mapped_ != MAP_FAILED);

	LatchManager *latch_manager = (LatchManager *)(mapped_ + 2*sizeof(page_id));

	PageManager *page_manager = new PageManager(fd_, (page_id *)mapped_+1);

	btree_ = new BTree(key_len, latch_manager, page_manager, (page_id *)mapped_);
}

bool MushroomDB::Put(KeySlice *key)
{
	return btree_->Put(key);
}

bool MushroomDB::Get(KeySlice *key)
{
	return btree_->Get(key);
}

bool MushroomDB::FindSingle(const char *file, const int total)
{
	std::ifstream in(file);
	assert(in.is_open());
	bool flag = btree_->KeyCheck(in, total);
	in.close();
	return flag;
}

bool MushroomDB::FindMultiple(const std::vector<std::string> &files, const int total)
{
	bool flag = true;
	std::vector<std::thread> vector;
	for (size_t i = 0; i != files.size(); ++i)
		vector.push_back(std::thread([&, i] {
			if (!FindSingle(files[i].c_str(), total))
				__sync_bool_compare_and_swap(&flag, true, false);
		}));
	for (auto &e : vector)
		e.join();
	return flag;
}

void MushroomDB::IndexSingle(const char *file, const int total)
{
	uint32_t key_len = btree_->KeyLen();
	ThreadPool *pool = new ThreadPool(new Queue(64, key_len));;

	char tmp[BTreePage::PageByte + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(file, O_RDONLY);
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
			pool->AddTask(&BTree::Put, btree_, key);
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	pool->Clear();
	delete pool;
}

void MushroomDB::IndexMultiple(const std::vector<std::string> &files, const int total)
{
	std::vector<std::thread> vector;
	for (size_t i = 0; i != files.size(); ++i)
		vector.push_back(std::thread([&, i] {
			uint32_t key_len = btree_->KeyLen();
			char tmp[BTreePage::PageByte + key_len] = {0};
			KeySlice *key = (KeySlice *)tmp;
			int fd = open(files[i].c_str(), O_RDONLY);
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
					Put(key);
					if (++count == total) {
						flag = false;
						break;
					}
					++i;
				}
			}
			close(fd);
		}));
	for (auto &e : vector)
		e.join();
}

bool MushroomDB::Close()
{
	btree_->Free();
	munmap(mapped_, BTreePage::PageSize * PageManager::LatchPages);
	close(fd_);
	return true;
}

} // namespace Mushroom
