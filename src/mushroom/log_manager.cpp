/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 14:46:11
**/

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cassert>
#include <cstring>

#include "log_manager.hpp"
#include "page.hpp"

namespace Mushroom {

uint32_t LogManager::LogPage;

LogManager::LogManager(const char *dir):cur_page_(0)
{
	char buf[32];
	size_t len = strlen(dir);
	memcpy(buf, dir, len);
	strcpy(buf + len, "/log");
	assert((fd_ = open(buf, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) != -1);
};

LogManager::~LogManager()
{
	assert(!ftruncate(fd_, 0));
	assert(!close(fd_));
}

void LogManager::SetManagerInfo(uint32_t log_page)
{
	LogPage = log_page;
}

Page* LogManager::NeedRecover()
{
	bool redo = true;
	uint32_t size = LogPage * Page::PageSize;
	if (!lseek(fd_, 0, SEEK_END)) {
		assert(!ftruncate(fd_, size));
		redo = false;
	}
	assert((mem_ = (char *)mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd_, 0))!=MAP_FAILED);
	return redo ? (Page *)mem_ : 0;
}

bool LogManager::NeedFlush()
{
	mutex_.Lock();
	bool flush = (cur_page_ == LogPage);
	mutex_.Unlock();
	return flush;
}

void LogManager::Logging(Page *page)
{
	mutex_.Lock();
	memcpy(mem_ + cur_page_ * Page::PageSize, page, Page::PageSize);
	assert(!msync(mem_ + cur_page_ * Page::PageSize, Page::PageSize, MS_SYNC));
	++cur_page_;
	mutex_.Unlock();
}

} // namespace Mushroom
