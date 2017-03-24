/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:13:52
**/

#include <unistd.h>
#include <sys/mman.h>

#include <iostream>

#include "page_manager.hpp"

namespace Mushroom {

void PageManager::PagePool::Initialize(page_id page_no)
{
	base_ = page_no & (~PoolMask);
	mem_ = new char[BTreePage::PageSize * SegSize];
}

BTreePage* PageManager::PagePool::GetPage(page_id page_no)
{
	return (BTreePage *)(mem_ + BTreePage::PageSize * (page_no & PoolMask));
}

PageManager::PageManager(int fd, page_id *cur):fd_(fd), cur_(cur), tot_(0)
{
	entries_ = new HashEntry[HashMask+1];
	pool_ = new PagePool[PoolSize];
}

BTreePage* PageManager::GetPage(page_id page_no)
{
	page_id base = page_no & (~PoolMask);
	page_id hash = (page_no >> SegBits) & HashMask;
	BTreePage *page = 0;

	entries_[hash].latch_.SpinWriteLock();
	if (entries_[hash].slot_) {
		PagePool *pool = pool_ + entries_[hash].slot_;
		for (; pool; pool = pool->next_)
			if (pool->base_ == base)
				break;
		if (pool) {
			page = pool->GetPage(page_no);
			entries_[hash].latch_.SpinReleaseWrite();
			return page;
		}
	}

	uint16_t victim = __sync_fetch_and_add(&tot_, 1) + 1;
	assert(victim < PoolSize);
	pool_[victim].Initialize(page_no);
	if (entries_[hash].slot_) {
		PagePool *next = pool_ + entries_[hash].slot_;
		next->prev_ = pool_ + victim;
		pool_[victim].next_ = next;
	}
	entries_[hash].slot_ = victim;
	page = pool_[victim].GetPage(page_no);

	entries_[hash].latch_.SpinReleaseWrite();
	assert(page);
	return page;
}

BTreePage* PageManager::NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_id page_no = __sync_fetch_and_add(cur_, 1);
	BTreePage *page = GetPage(page_no);
	page->Initialize(page_no, type, key_len, level, degree);
	return page;
}

bool PageManager::Free()
{
	for (uint16_t i = 0; i != tot_; ++i)
		delete [] pool_[i].mem_;
	return true;
}

PageManager::~PageManager()
{
	delete [] pool_;
	delete [] entries_;
}

} // namespace Mushroom
