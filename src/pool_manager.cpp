/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:13:52
**/

#include "latch.hpp"
#include "page.hpp"
#include "page_pool.hpp"

#include "pool_manager.hpp"

namespace Mushroom {

uint32_t PoolManager::HashMask;
uint32_t PoolManager::PoolSize;

void PoolManager::SetManagerInfo(uint32_t page_size, uint32_t pool_size, uint32_t hash_bits,
	uint32_t seg_bits)
{
	Page::SetPageInfo(page_size);
	PagePool::SetPoolInfo(seg_bits);
	PoolSize = pool_size;
	HashMask = (1 << hash_bits) - 1;
}

PoolManager::PoolManager():cur_(0), tot_(0)
{
	entries_ = new HashEntry[HashMask+1];
	pool_ = new PagePool[PoolSize];
}

PoolManager::~PoolManager()
{
	delete [] pool_;
	delete [] entries_;
}

void PoolManager::Link(uint16_t hash, uint16_t victim)
{
	PagePool *pool = pool_ + victim;

	uint16_t slot = entries_[hash].slot_;

	if (slot) {
		PagePool *next = pool_ + slot;
		next->prev_ = pool_ + victim;
		pool->next_ = next;
	}
	entries_[hash].slot_ = victim;
	pool->prev_ = 0;
	pool->hash_ = hash;
}

Page* PoolManager::GetPage(page_id page_no)
{
	page_id base = page_no & ~PagePool::SegMask;
	page_id hash = (page_no >> PagePool::SegBits) & HashMask;
	Page *page = 0;

	#ifndef NOLATCH
	entries_[hash].latch_.Lock();
	#endif

	uint16_t slot = entries_[hash].slot_;
	if (slot) {
		PagePool *pool = pool_ + slot;
		for (; pool; pool = pool->next_)
			if (pool->base_ == base)
				break;
		if (pool) {
			page = pool->GetPage(page_no);
			#ifndef NOLATCH
			entries_[hash].latch_.Unlock();
			#endif
			return page;
		}
	}

	uint16_t victim = __sync_fetch_and_add(&tot_, 1) + 1;

	if (victim < PoolSize) {
		pool_[victim].Initialize(page_no);
		Link(hash, victim);
		page = pool_[victim].GetPage(page_no);
		#ifndef NOLATCH
		entries_[hash].latch_.Unlock();
		#endif
		return page;
	}
	assert(0);
}

Page* PoolManager::NewPage(uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_id page_no = __sync_fetch_and_add(&cur_, 1);
	Page *page = GetPage(page_no);
	page->Initialize(page_no, type, key_len, level, degree);
	return page;
}

bool PoolManager::Free()
{
	for (uint16_t i = 0; i != tot_; ++i)
		delete [] pool_[i].mem_;
	return true;
}

} // namespace Mushroom
