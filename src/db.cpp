/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:34:43
**/

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <thread>
#include <iostream>

#include "db.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, const int key_len)
{
	// assert(key_len <= 256);
	// assert(!chdir(".."));
	// assert(mkdir(name, S_IRUSR | S_IWUSR | S_IROTH) >= 0);
	// assert(!chdir(name));

	// assert(access("index", F_OK));
	// assert(creat("index", O_RDWR) > 0);
	// int fd = open("index", O_RDWR);
	// assert(fd > 0);
	// btree_ = new BTree(fd, key_len);

	btree_ = new BTree(-1, key_len);

	pool_ = new ThreadPool(new Queue(1024, key_len));
}

Status MushroomDB::Put(KeySlice *key)
{
	pool_->AddTask(&BTree::Put, btree_, key);
	return Success;
}

Status MushroomDB::Get(KeySlice *key)
{
	return btree_->Get(key);
}

Status MushroomDB::FindSingle(const char *file, const int total)
{
	std::ifstream in(file);
	assert(in.is_open());
	bool flag = btree_->KeyCheck(in, total);
	in.close();
	return flag ? Success : Fail;
}

Status MushroomDB::FindMultiple(const std::vector<std::string> &files, const int total)
{
	for (size_t i = 0; i != files.size(); ++i)
		if (FindSingle(files[i].c_str(), total) == Fail)
			return Fail;
	return Success;
}

Status MushroomDB::IndexSingle(const char *file, const int total)
{
	uint32_t key_len = btree_->KeyLen();
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
			Put(key);
			if (++count == total) {
				flag = false;
				break;
			}
			if (!(count % 1000000)) {
				std::cout << count << " " << BTreePage::current << std::endl;
			}
			++i;
		}
	}
	close(fd);
	pool_->Clear();
	return Success;
}

Status MushroomDB::IndexMultiple(const std::vector<std::string> &files, const int total)
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
					btree_->Put(key);
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
	return Success;
}

Status MushroomDB::Close()
{
	btree_->Close();
	return Success;
}

} // namespace Mushroom
