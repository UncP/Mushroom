/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-21 16:52:52
**/

#include "latch_manager.hpp"
#include "latch.hpp"

namespace Mushroom {

LatchManager::LatchManager():deployed_(0), victim_(0)
{
	entries_ = new HashEntry[total];
	latches_ = new Latch[total];
}

LatchManager::~LatchManager()
{
	delete [] latches_;
	delete [] entries_;
}

void LatchManager::Link(uint16_t hashidx, uint16_t victim, page_t page_no)
{
	Latch *latch = latches_ + victim;

	if ((latch->next_ = entries_[hashidx].GetSlot()))
		latches_[latch->next_].prev_ = victim;

	entries_[hashidx].SetSlot(victim);

	latch->id_   = page_no;
	latch->hash_ = hashidx;
	latch->prev_ = 0;
}

Latch* LatchManager::GetLatch(page_t page_no)
{
	uint16_t hashidx = page_no & mask;
	Latch *latch;

  entries_[hashidx].Lock();

	uint16_t slot = entries_[hashidx].GetSlot(), avail = 0;
	for (; slot; slot = latch->next_) {
		latch = latches_ + slot;
		if (page_no == latch->id_)
			break;
		if (!latch->pin_.get() && !avail)
			avail = slot;
	}

	if (slot || (slot = avail)) {
		latch = latches_ + slot;
		latch->Pin();
		latch->id_ = page_no;
		entries_[hashidx].Unlock();
		return latch;
  }

	uint16_t victim = ++deployed_;

	if (victim < total) {
		latch = latches_ + victim;
		latch->Pin();
		Link(hashidx, victim, page_no);
		entries_[hashidx].Unlock();
		return latch;
	}

	--deployed_;

  for (;;) {
		victim = victim_++;

		if ((victim %= total))
			latch = latches_ + victim;
		else
			continue;

		if (latch->pin_.get() || !latch->busy_.TryLock())
			continue;

		uint16_t idx = latch->hash_;

		if (!entries_[idx].TryLock()) {
			latch->busy_.Unlock();
			continue;
		}

		if (latch->pin_.get()) {
			entries_[idx].Unlock();
			latch->busy_.Unlock();
			continue;
		}

		if (latch->prev_)
			latches_[latch->prev_].next_ = latch->next_;
		else
			entries_[idx].SetSlot(latch->next_);

		if (latch->next_)
			latches_[latch->next_].prev_ = latch->prev_;

		entries_[idx].Unlock();
		latch->Pin();
		Link(hashidx, victim, page_no);
		entries_[hashidx].Unlock();
		latch->busy_.Unlock();
		return latch;
  }
}

} // namespace Mushroom
