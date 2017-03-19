/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-21 16:52:52
**/

#include "latch_manager.hpp"

namespace Mushroom {

LatchManager::LatchManager():deployed_(0), victim_(0)
{
	latch_set_ = new LatchSet[mask-1];
	latches_   = new Latch[total];
}

void LatchManager::Link(uint16_t hashidx, uint16_t victim, page_id page_no)
{
	Latch *latch = latches_ + victim;

	if ((latch->next_ = latch_set_[hashidx].slot_))
		latches_[latch->next_].prev_ = victim;

	latch_set_[hashidx].slot_ = victim;

	latch->page_no_ = page_no;
	latch->hash_    = hashidx;
	latch->prev_    = 0;
}

Latch* LatchManager::GetLatch(page_id page_no)
{
	uint16_t hashidx = page_no % mask;
	Latch *latch;

	latch_set_[hashidx].latch_.SpinReadLock();

	uint16_t avail = 0, slot = latch_set_[hashidx].slot_;
	for (; slot; slot = latch->next_) {
		latch = latches_ + slot;
		if (page_no == latch->page_no_)
			break;
	}

	if (slot) latch->Pin();

	latch_set_[hashidx].latch_.SpinReleaseRead();

	if (slot) return latch;

  latch_set_[hashidx].latch_.SpinWriteLock();

	slot = latch_set_[hashidx].slot_;
	for (; slot; slot = latch->next_) {
		latch = latches_ + slot;
		if (page_no == latch->page_no_)
			break;
		if (!latch->pin_ && !avail)
			avail = slot;
	}

	if (slot || (slot = avail)) {
		latch = latches_ + slot;
		latch->Pin();
		latch->page_no_ = page_no;
		latch_set_[hashidx].latch_.SpinReleaseWrite();
		return latch;
  }

	uint16_t victim = __sync_fetch_and_add(&deployed_, 1) + 1;

	if (victim < total) {
		latch = latches_ + victim;
		latch->Pin();
		Link(hashidx, victim, page_no);
		latch_set_[hashidx].latch_.SpinReleaseWrite();
		return latch;
	}

	victim = __sync_fetch_and_add(&deployed_, -1);

  for (;;) {
		victim = __sync_fetch_and_add(&victim_, 1);

		if ((victim %= total))
			latch = latches_ + victim;
		else
			continue;

		if (latch->pin_ || !latch->busy_.SpinWriteTry())
			continue;

		uint16_t idx = latch->hash_;

		if (!latch_set_[idx].latch_.SpinWriteTry()) {
			latch->busy_.SpinReleaseWrite();
			continue;
		}

		if (latch->pin_) {
			latch->busy_.SpinReleaseWrite();
			latch_set_[idx].latch_.SpinReleaseWrite();
			continue;
		}

		if (latch->prev_)
			latches_[latch->prev_].next_ = latch->next_;
		else
			latch_set_[idx].slot_ = latch->next_;

		if (latch->next_)
			latches_[latch->next_].prev_ = latch->prev_;

		latch_set_[idx].latch_.SpinReleaseWrite();
		latch->Pin();
		Link(hashidx, victim, page_no);
		latch_set_[hashidx].latch_.SpinReleaseWrite();
		latch->busy_.SpinReleaseWrite();
		return latch;
  }
}

LatchManager::~LatchManager()
{
	delete [] latches_;
	delete [] latch_set_;
}

} // namespace Mushroom
