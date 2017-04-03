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

PoolManager::PoolManager(uint32_t page_size, uint32_t pool_size, uint8_t hash_bits,
	uint8_t seg_bits)
:pool_size_(pool_size), hash_mask_((1<<hash_bits)-1), cur_(0), tot_(0)
{
	Page::SetPageInfo(page_size);
	PagePool::SetPoolInfo(seg_bits);

	entries_ = new HashEntry[hash_mask_+1];
	pool_ = new PagePool[pool_size_];
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

Page* PoolManager::GetPage(page_id page_no)
{
	page_id base = page_no & ~PagePool::SegMask;
	page_id hash = (page_no >> PagePool::SegBits) & hash_mask_;
	Page *page = 0;

	#ifndef NOLATCH
	entries_[hash].latch_.SpinWriteLock();
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
			entries_[hash].latch_.SpinReleaseWrite();
			#endif
			return page;
		}
	}

	uint16_t victim = __sync_fetch_and_add(&tot_, 1) + 1;

	if (victim < pool_size_) {
		pool_[victim].Initialize(page_no);
		Link(hash, victim);
		page = pool_[victim].GetPage(page_no);
		#ifndef NOLATCH
		entries_[hash].latch_.SpinReleaseWrite();
		#endif
		return page;
	}
	assert(0);
}

Page* PoolManager::NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree)
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

PoolManager::~PoolManager()
{
	delete [] pool_;
	delete [] entries_;
}

} // namespace Mushroom
