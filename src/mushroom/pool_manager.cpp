/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-03-19 13:13:52
**/

#include <unistd.h>
#include <cassert>
#include <cstring>

#include "latch.hpp"
#include "page.hpp"
#include "page_pool.hpp"
#include "pool_manager.hpp"
#include "latch_manager.hpp"

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

PoolManager::PoolManager():cur_page_(0), total_pool_(0)
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

	uint16_t slot = entries_[hash].GetSlot();
	if (slot)
		pool->next_ = pool_ + slot;

	entries_[hash].SetSlot(victim);
}

Page* PoolManager::GetPage(page_t page_no)
{
	page_t base = page_no & ~PagePool::SegMask;
	page_t hash = (page_no >> PagePool::SegBits) & HashMask;
	Page *page = 0;

	entries_[hash].Lock();

	uint16_t slot = entries_[hash].GetSlot();
	if (slot) {
		PagePool *pool = pool_ + slot;
		for (; pool; pool = pool->next_)
			if (pool->base_ == base)
				break;
		if (pool) {
			page = pool->GetPage(page_no);
			entries_[hash].Unlock();
			return page;
		}
	}

	uint16_t victim = ++total_pool_;
	assert(victim < PoolSize);

	pool_[victim].Initialize(base);
	Link(hash, victim);
	page = pool_[victim].GetPage(page_no);
	entries_[hash].Unlock();
	return page;
}

Page* PoolManager::NewPage(uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	page_t page_no = cur_page_++;
	Page *page = GetPage(page_no);
	return new (page) Page(page_no, type, key_len, level, degree);
}

void PoolManager::Free()
{
	for (uint16_t i = 1, end = total_pool_.get(); i <= end; ++i)
		delete [] pool_[i].mem_;
	printf("memory usage: %u %.2f\n", total_pool_.get(),
		float(total_pool_.get()) * PagePool::SegSize / 256);
}

} // namespace Mushroom
