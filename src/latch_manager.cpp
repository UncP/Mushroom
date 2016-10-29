/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-21 16:52:52
**/

#include <cassert>

#include "latch_manager.hpp"

namespace Mushroom {

SharedLock* LatchSet::FindLock(page_id page_no)
{
	// std::lock_guard<std::mutex> lock(mutex_);
	for (SharedLock *lk = head_; lk; lk = lk->Next())
		if (lk->Id() == page_no)
			return lk;
	return nullptr;
}

void LatchSet::PinLock(SharedLock *lk)
{
	// std::lock_guard<std::mutex> lock(mutex_);
	lk->Link(nullptr, head_);
}

void LatchSet::UnpinLock(SharedLock *lk)
{
	// std::lock_guard<std::mutex> lock(mutex_);
	if (lk == head_) {
		assert(!head_->Prev());
		head_ = lk->Next();
		return ;
	}
	for (SharedLock *curr = head_->Next(); curr; curr = curr->Next())
		if (curr == lk) {
			lk->Detach();
			return ;
		}
	assert(0);
}

void LatchManager::LockShared(page_id page_no)
{
	int index = page_no & Mask;
	std::lock_guard<std::mutex> guard(latch_mutex_[index]);
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	if (!lock) {
		lock = AllocateFree(page_no);
		assert(lock->Id() == page_no);
		latch_set_[index].PinLock(lock);
	}
	lock->LockShared();
}

void LatchManager::UnlockShared(page_id page_no)
{
	int index = page_no & Mask;
	std::lock_guard<std::mutex> guard(latch_mutex_[index]);
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	lock->UnlockShared();
}

void LatchManager::Lock(page_id page_no)
{
	int index = page_no & Mask;
	std::lock_guard<std::mutex> guard(latch_mutex_[index]);
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	if (!lock) {
		lock = AllocateFree(page_no);
		assert(lock->Id() == page_no);
		latch_set_[index].PinLock(lock);
	}
	lock->Lock();
}

void LatchManager::Unlock(page_id page_no)
{
	int index = page_no & Mask;
	std::lock_guard<std::mutex> guard(latch_mutex_[index]);

	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	lock->Unlock();
	latch_set_[index].UnpinLock(lock);
}

void LatchManager::Upgrade(page_id page_no)
{
	int index = page_no & Mask;
	std::lock_guard<std::mutex> guard(latch_mutex_[index]);
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	lock->Upgrade();
}

void LatchManager::Downgrade(page_id page_no)
{
	int index = page_no & Mask;
	std::lock_guard<std::mutex> guard(latch_mutex_[index]);
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	lock->Downgrade();
}

LatchManager::LatchManager()
{
	free_ = new SharedLock[Max];
	assert(free_);
}

SharedLock* LatchManager::AllocateFree(page_id id)
{
	std::lock_guard<std::mutex> lock(mutex_);
	for (int i = 0; i != Max; ++i)
		if (!free_[i].Id()) {
			free_[i].SetId(id);
			return &free_[i];
		}
	assert(0);
}

} // namespace Mushroom
