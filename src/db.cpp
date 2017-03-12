/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time: 2016-10-10 15:34:43
**/

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "db.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, const int key_len)
:name_(std::string(name)), pool_(nullptr)
{
	assert(key_len <= 256);
	assert(!chdir(".."));
	assert(mkdir(name, S_IRUSR | S_IWUSR | S_IROTH) >= 0);
	assert(!chdir(name));

	assert(access("index", F_OK));
	assert(creat("index", O_RDWR) > 0);
	int fd = open("index", O_RDWR);
	assert(fd > 0);
	btree_ = new BTree(fd, key_len);

	#ifndef SingleThread
		pool_ = new ThreadPool(new Queue(1024, key_len));
	#endif
}

Status MushroomDB::Put(KeySlice *key)
{
	#ifndef SingleThread
	pool_->AddTask(&BTree::Put, btree_, key);
	return Success;
	#else
	return btree_->Put(key);
	#endif
}

Status MushroomDB::Close()
{
	btree_->Close();
	return Success;
}

} // namespace Mushroom
