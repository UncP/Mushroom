/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:13:52
**/

#define __USE_LARGEFILE64
#define __USE_FILE_OFFSET64

#include <unistd.h>
#include <sys/mman.h>

#include "pool_manager.hpp"

namespace Mushroom {

uint32_t PoolManager::PoolSize;
uint32_t PoolManager::HashMask;

void PoolManager::SetPoolManagerInfo(uint32_t page_size, uint32_t pool_size, uint8_t hash_bits,
	uint8_t seg_bits)
{
	PoolSize = pool_size;
	HashMask = (1 << hash_bits) - 1;
	BTreePage::SetPageInfo(page_size);
	PagePool::SetPoolInfo(seg_bits);
}

PoolManager::PoolManager(int fd, page_id *cur)
:fd_(fd), cur_(cur), tot_(0)
{
	entries_ = new HashEntry[HashMask+1];
	pool_ = new PagePool[PoolSize];
}

void PoolManager::Link(uint16_t hash, uint16_t victim)
{
	uint16_t slot = entries_[hash].slot_;
	if (slot) {
		PagePool *next = pool_ + slot;
		next->prev_ = pool_ + victim;
		pool_[victim].next_ = next;
	}
	entries_[hash].slot_ = victim;
	pool_[victim].prev_ = 0;
}

BTreePage* PoolManager::GetPage(page_id page_no)
{
	page_id base = page_no & ~PagePool::SegMask;
	page_id hash = (page_no >> PagePool::SegBits) & HashMask;
	BTreePage *page = 0;

	entries_[hash].latch_.SpinWriteLock();

	uint16_t slot = entries_[hash].slot_;
	if (slot) {
		PagePool *pool = pool_ + slot;
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

	if (victim < PoolSize) {
		pool_[victim].Initialize(page_no);
		Link(hash, victim);
		page = pool_[victim].GetPage(page_no);
		entries_[hash].latch_.SpinReleaseWrite();
		return page;
	}
	assert(0);
	// __sync_fetch_and_add(&tot_, -1);

	// for (; --victim;) {
	// 	PagePool *pool = pool_ + victim;
	// 	uint16_t idx = (pool->base_ >> PagePool::SegBits) & HashMask;

	// 	if (!entries_[idx].latch_.SpinWriteTry())
	// 		continue;
	// }
}

BTreePage* PoolManager::NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_id page_no = __sync_fetch_and_add(cur_, 1);
	BTreePage *page = GetPage(page_no);
	page->Initialize(page_no, type, key_len, level, degree);
	return page;
}

bool PoolManager::Free()
{
	for (uint16_t i = 0; i != tot_; ++i)
		delete [] pool_[i].mem_;
	return true;
}

PoolManager::~PoolManager()
{
	delete [] pool_;
	delete [] entries_;
}

} // namespace Mushroom
