/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 14:46:11
**/

#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

#include "log_manager.hpp"

namespace Mushroom {

LogManager::LogManager(const char *dir)
{
	char buf[32];
	size_t len = strlen(dir);
	memcpy(buf, dir, len);
	strcpy(buf + len, "/log");
	assert((fd_ = open(buf, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) != -1);
};

LogManager::~LogManager()
{
	close(fd_);
}

/*
void LogManager::Redo()
{
	if (lseek(fd_, 0, SEEK_END)) {

	}

}
*/

} // namespace Mushroom
