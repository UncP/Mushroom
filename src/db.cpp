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

Status MushroomDB::Open(const char *file, const int key_len)
{
	assert(!chdir(".."));
	file_ = std::string(file);

	assert(mkdir(file, S_IRUSR | S_IWUSR | S_IROTH) >= 0);
	assert(chdir(file) == 0);

	assert(access("index", F_OK));
	int fd = creat("index", O_RDWR);
	assert(fd > 0);

	assert(btree_ = new BTree());
	assert(btree_->Init(fd, key_len));

	return Success;
}

Status MushroomDB::Close()
{
	btree_->Close();
	return Success;
}

} // namespace Mushroom
