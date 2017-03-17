/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-21 16:52:52
**/

#include <cassert>
#include <iostream>

#include "latch_manager.hpp"

namespace Mushroom {

LatchManager::LatchManager():deployed_(0), next_(0)
{
	latch_set_ = new LatchSet[mask-1];
	latches_   = new Latch[total];
}

void LatchManager::Link(uint32_t idx, uint32_t cur, page_id id)
{
	Latch *latch = latches_ + cur;
	if ((latch->next_ = latch_set_[idx].slot_))
		latches_[latch->next_].prev_ = cur;
	latch_set_[idx].slot_ = cur;
	latch->id_   = id;
	latch->hash_ = idx;
	latch->prev_ = 0;
}

Latch* LatchManager::GetLatch(page_id id)
{
	uint32_t idx = id % mask;
	Latch *latch = nullptr;
	pthread_rwlock_rdlock(latch_set_[idx].lock_);
	uint32_t avail = 0, slot = latch_set_[idx].slot_;
	for (; slot; slot = latch->next_) {
		latch = latches_ + slot;
		if (latch->id_ == id)
			break;
	}
	if (slot) ++latch->pin_;
	pthread_rwlock_unlock(latch_set_[idx].lock_);
	if (slot) {
		latch->page_ = BTreePage::GetPage(id);
		return latch;
	}

	pthread_rwlock_wrlock(latch_set_[idx].lock_);
	slot = latch_set_[idx].slot_;
	for (; slot; slot = latch->next_) {
		latch = latches_ + slot;
		if (latch->id_ == id)
			break;
		if (!latch->pin_ && !avail)
			avail = slot;
	}

	if (slot || (slot = avail)) {
		latch = latches_ + slot;
		++latch->pin_;
		latch->id_  = id;
		pthread_rwlock_unlock(latch_set_[idx].lock_);
		latch->page_ = BTreePage::GetPage(id);
		return latch;
	}

	uint32_t next = deployed_++ + 1;
	if (next < total) {
		latch = latches_ + next;
		++latch->pin_;
		Link(idx, next, id);
		pthread_rwlock_unlock(latch_set_[idx].lock_);
		latch->page_ = BTreePage::GetPage(id);
		return latch;
	}
	--deployed_;
	for (;;) {
		next = deployed_++;
		if ((next %= total))
			latch = latches_ + next;
		else
			continue;
		if (latch->pin_ || pthread_rwlock_trywrlock(latch->busy_))
			continue;

		uint32_t nidx = latch->hash_;
		if (pthread_rwlock_trywrlock(latch_set_[nidx].lock_)) {
			pthread_rwlock_unlock(latch->busy_);
			continue;
		}
		if (latch->pin_) {
			pthread_rwlock_unlock(latch_set_[nidx].lock_);
			pthread_rwlock_unlock(latch->busy_);
			continue;
		}

		if (latch->prev_)
			latches_[latch->prev_].next_ = latch->next_;
		else
			latch_set_[nidx].slot_ = latch->next_;

		if (latch->next_)
			latches_[latch->next_].prev_ = latch->prev_;
		pthread_rwlock_unlock(latch_set_[nidx].lock_);
		++latch->pin_;
		Link(idx, next, id);
		pthread_rwlock_unlock(latch->busy_);
		pthread_rwlock_unlock(latch_set_[idx].lock_);
		latch->page_ = BTreePage::GetPage(id);
		return latch;
	}
}

} // namespace Mushroom
