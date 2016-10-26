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

MushroomDB::MushroomDB(const char *file, const int key_len):file_(std::string(file)) {
	assert(!chdir(".."));
	assert(mkdir(file, S_IRUSR | S_IWUSR | S_IROTH) >= 0);
	assert(!chdir(file));

	assert(access("index", F_OK));
	assert(creat("index", O_RDWR) > 0);
	int fd = open("index", O_RDWR);
	assert(fd > 0);

	assert(btree_ = new BTree(fd, key_len));
}

Status MushroomDB::Close()
{
	btree_->Close();
	return Success;
}

} // namespace Mushroom
