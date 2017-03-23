/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:13:52
**/

#define __USE_LARGEFILE64   1
#define __USE_FILE_OFFSET64 1

#include <unistd.h>
#include <sys/mman.h>
#include <iostream>

#include "page_manager.hpp"

namespace Mushroom {

PageManager::PageManager(int fd, page_id *cur):fd_(fd), cur_(cur)
{
	if (fd_ == -1) {
		mem_ = new char[BTreePage::PageSize * 80000];
	} else {
		if (!*cur_) {
			char tmp[BTreePage::PageSize] = {0};
			assert(pwrite(fd_, tmp, BTreePage::PageSize, 12*BTreePage::PageSize)
				== BTreePage::PageSize);
		}
		mem_ = (char *)mmap(0, 10*BTreePage::PageSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_,
			BTreePage::PageSize * LatchPages);
		assert(mem_ != MAP_FAILED);
	}
}

BTreePage* PageManager::GetPage(page_id page_no)
{
	return (BTreePage *)(mem_ + page_no * BTreePage::PageSize);
}

BTreePage* PageManager::NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_id page_no = __sync_fetch_and_add(cur_, 1);
	BTreePage *page = (BTreePage *)(mem_ + page_no * BTreePage::PageSize);
	page->Initialize(page_no, type, key_len, level, degree);
	return page;
}

bool PageManager::Free()
{
	if (fd_ == -1) {
		delete [] mem_;
	} else {
		munmap(mem_, 10*BTreePage::PageSize);
	}
	cur_ = 0;
	mem_ = 0;
	return true;
}

} // namespace Mushroom
