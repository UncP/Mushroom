/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-21 16:52:52
**/

#include <cassert>
#include <iostream>

#include "latch_manager.hpp"

namespace Mushroom {

SharedLock* LatchSet::FindLock(page_id page_no)
{
	std::cout << "find lock\n";
	for (SharedLock *lk = head_; lk; lk = lk->Next())
		if (lk->Id() == page_no)
			return lk;
	return nullptr;
}

void LatchSet::PinLock(SharedLock *lk)
{
	lk->Link(nullptr, head_);
	head_ = lk;
}

void LatchSet::UnpinLock(SharedLock *lk)
{
	if (lk == head_) {
		assert(!head_->Prev());
		head_ = lk->Next();
		lk->SetId(0);
		return ;
	}
	assert(0);
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
	latch_mutex_[index].lock();
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	if (!lock) {
		lock = AllocateFree(page_no);
		assert(lock->Id() == page_no);
		latch_set_[index].PinLock(lock);
	}
	latch_mutex_[index].unlock();
	lock->LockShared();
	std::cout << "lock shared\n";
}

void LatchManager::UnlockShared(page_id page_no)
{
	int index = page_no & Mask;
	latch_mutex_[index].lock();
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	if (!lock) std::cout << count_;
	assert(lock);
	latch_mutex_[index].unlock();
	lock->UnlockShared();
	--count_;
	std::cout << "unlock shared\n";
}

void LatchManager::Lock(page_id page_no)
{
	int index = page_no & Mask;
	latch_mutex_[index].lock();
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	if (!lock) {
		lock = AllocateFree(page_no);
		assert(lock->Id() == page_no);
		latch_set_[index].PinLock(lock);
	}
	latch_mutex_[index].unlock();
	++count_;
	lock->Lock();
	std::cout << "lock\n";
}

void LatchManager::Unlock(page_id page_no)
{
	int index = page_no & Mask;
	latch_mutex_[index].lock();
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	lock->Unlock();
	latch_set_[index].UnpinLock(lock);
	latch_mutex_[index].unlock();
	std::cout << "unlock\n";
}

void LatchManager::Upgrade(page_id page_no)
{
	int index = page_no & Mask;
	latch_mutex_[index].lock();
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	latch_mutex_[index].unlock();
	lock->Upgrade();
	std::cout << "upgrade\n";
}

void LatchManager::Downgrade(page_id page_no)
{
	int index = page_no & Mask;
	latch_mutex_[index].lock();
	SharedLock *lock = latch_set_[index].FindLock(page_no);
	assert(lock);
	latch_mutex_[index].unlock();
	lock->Downgrade();
	std::cout << "downgrade\n";
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
			std::cout << i << std::endl;
			free_[i].SetId(id);
			return &free_[i];
		}
	assert(0);
}

} // namespace Mushroom
