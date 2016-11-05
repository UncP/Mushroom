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

LatchSet::LatchSet()
{
	for (int i = 0; i != Max; ++i) {
		locks_[i].Link(free_);
		free_ = &locks_[i];
	}
}

SharedLock* LatchSet::GetLock(page_id page_no)
{
	std::unique_lock<std::mutex> guard(mutex_);
	for (int i = 0; i != Max; ++i) {
		if (locks_[i].Id() == page_no)
			return &locks_[i];
	}
	for (int i = 0; i != Max; ++i)
		if (!locks_[i].Users() && !locks_[i].Occupy()) {
			locks_[i].SetId(page_no);
			return &locks_[i];
		}
	assert(0);
}

void LatchManager::LockShared(page_id page_no)
{
	int index = page_no & Mask;
	SharedLock *lock = latch_set_[index].GetLock(page_no);
	if (lock->Id() != page_no) {
		std::cout << lock->Id() << " " << page_no << std::endl;
		std::cout << lock->Users() << std::endl;
	}
	assert(lock->Id() == page_no);
	lock->LockShared();
	// std::cout << "lock shared\n";
}

void LatchManager::UnlockShared(page_id page_no)
{
	int index = page_no & Mask;
	SharedLock *lock = latch_set_[index].GetLock(page_no);
	assert(lock->Id() == page_no);
	lock->UnlockShared();
	// std::cout << "unlock shared\n";
}

void LatchManager::Lock(page_id page_no)
{
	int index = page_no & Mask;
	SharedLock *lock = latch_set_[index].GetLock(page_no);
	if (lock->Id() != page_no) {
		std::cout << lock->Id() << " " << page_no << std::endl;
		std::cout << lock->Users() << std::endl;
	}
	assert(lock->Id() == page_no);
	lock->Lock();
	// std::cout << "lock\n";
}

void LatchManager::Unlock(page_id page_no)
{
	int index = page_no & Mask;
	SharedLock *lock = latch_set_[index].GetLock(page_no);
	assert(lock->Id() == page_no);
	lock->Unlock();
	// latch_set_[index].FreeLock(page_no);
}

void LatchManager::Upgrade(page_id page_no)
{
	int index = page_no & Mask;
	SharedLock *lock = latch_set_[index].GetLock(page_no);
	assert(lock->Id() == page_no);
	lock->Upgrade();
	// std::cout << "upgrade\n";
}

void LatchManager::Downgrade(page_id page_no)
{
	int index = page_no & Mask;
	SharedLock *lock = latch_set_[index].GetLock(page_no);
	assert(lock->Id() == page_no);
	lock->Downgrade();
	// std::cout << "downgrade\n";
}

} // namespace Mushroom
