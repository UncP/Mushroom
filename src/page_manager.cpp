/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:13:52
**/

#include <unistd.h>
#include <sys/mman.h>

#include "page_manager.hpp"

namespace Mushroom {

void PageManager::PagePool::Initialize(page_id page_no)
{
	base_ = page_no & ~PoolMask;
	mem_ = new char[BTreePage::PageSize * Segment];
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
}

BTreePage* PageManager::NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_id page_no = __sync_fetch_and_add(cur_, 1);
	BTreePage *page;

	if (!(page_no & PoolMask)) {
		uint16_t victim = __sync_fetch_and_add(&tot_, 1);
		assert(victim < PoolSize);
		pool_[victim].Initialize(page_no);
		uint16_t hash = (page_no >> SegBits) & HashMask;
		entries_[hash].latch_.SpinWriteLock();
		if (entries_[hash].slot_) {
			PagePool *next = entries_[hash].slot_;
			next->prev_ = pool_ + victim;
			pool_[victim].next_ = next;
		}
		entries_[hash].slot_ = victim;
		entries_[hash].latch_.SpinReleaseWrite();
	}

	BTreePage *page = GetPage(page_no);
	page->Initialize(page_no, type, key_len, level, degree);
	return page;
}

bool PageManager::Free()
{
	return true;
}

} // namespace Mushroom
