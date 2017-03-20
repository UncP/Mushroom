/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:13:52
**/

#include <sys/mman.h>

#include "page_manager.hpp"

namespace Mushroom {

PageManager::PageManager(int fd, page_id tot):cur_(0), tot_(0), mem_(0)
{
	if (!tot) {
		mem_ = new char[BTreePage::PageSize * 80000];
	} else {
		mem_ = (char *)mmap(0, tot * BTreePage::PageSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	}
}

BTreePage* PageManager::GetPage(page_id page_no)
{
	return (BTreePage *)(mem_ + page_no * BTreePage::PageSize);
}

BTreePage* PageManager::NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_id page_no = __sync_fetch_and_add(&cur_, 1);
	BTreePage *page = (BTreePage *)(mem_ + page_no * BTreePage::PageSize);
	page->Initialize(page_no, type, key_len, level, degree);
	return page;
}

bool PageManager::Free()
{
	if (!tot_) {
		delete [] mem_;
	} else {
		munmap((void *)mem_, tot_ * BTreePage::PageSize);
	}
	cur_ = 0;
	tot_ = 0;
	mem_ = 0;
	return true;
}

PageManager::~PageManager()
{
	if (mem_) Free();
}

} // namespace Mushroom
