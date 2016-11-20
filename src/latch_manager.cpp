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

Latch* LatchSet::GetLatch(page_id id)
{
	Latch *latch = nullptr;
	lock_.LockShared();
	for (int i = 0; i != Max; ++i) {
		if (latches_[i].Id() == id) {
			latches_[i].Pin();
			latch = &latches_[i];
			break;
		}
	}
	lock_.UnlockShared();
	if (latch) return latch;
	lock_.Lock();
	for (int i = 0; i != Max; ++i) {
		if (latches_[i].Id() == id) {
			latches_[i].Pin();
			latch = &latches_[i];
			break;
		}
		if (latches_[i].Free()) {
			latches_[i].SetId(id);
			latches_[i].Pin();
			latch = &latches_[i];
			break;
		}
	}
	lock_.Unlock();
	if (latch) return latch;
	assert(0);
}

Latch* LatchManager::GetLatch(page_id id)
{
	return latch_set_[id & Mask].GetLatch(id);
}

std::string LatchSet::ToString() const
{
	std::string res;
	for (int i = 0; i != Max; ++i)
		res += latches_[i].ToString();
	return std::move(res);
}

std::string LatchManager::ToString() const
{
	std::string res;
	for (int i = 0; i != Hash; ++i)
		res += latch_set_[i].ToString() + "\n";
	return std::move(res);
}

} // namespace Mushroom
