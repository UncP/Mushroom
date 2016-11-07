/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 15:34:43
**/

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "db.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, const int key_len):name_(std::string(name))
{
	assert(!chdir(".."));
	assert(mkdir(name, S_IRUSR | S_IWUSR | S_IROTH) >= 0);
	assert(!chdir(name));

	assert(access("index", F_OK));
	assert(creat("index", O_RDWR) > 0);
	int fd = open("index", O_RDWR);
	assert(fd > 0);
	assert(btree_ = new BTree(fd, key_len));

	assert(access("data", F_OK));
	assert(creat("data", O_RDWR) > 0);
	fd = open("data", O_RDWR);
	assert(fd > 0);
	assert(data_pager_ = new DataPager(fd));
}

Status MushroomDB::Put(const Slice &key, const Slice &val)
{

}

Status MushroomDB::Close()
{
	btree_->Close();
	return Success;
}

} // namespace Mushroom
