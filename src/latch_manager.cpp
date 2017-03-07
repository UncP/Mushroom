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

Latch* LatchSet::GetLatch(page_id id)
{
	Latch *latch = nullptr;
	mutex_.lock();
	for (int i = 0; i != Max; ++i) {
		if (latches_[i].Id() == id) {
			latch = &latches_[i];
			break;
		}
		if (!latch && latches_[i].Free())
			latch = &latches_[i];
	}
	assert(latch);
	latch->SetId(id);
	latch->Pin();
	mutex_.unlock();
	return latch;
}

Latch* LatchManager::GetLatch(page_id id)
{
	Latch *latch = latch_set_[id & Mask].GetLatch(id);
	latch->page_ = BTreePage::GetPage(id);
	return latch;
}

std::string LatchSet::ToString() const
{
	std::string res;
	for (int i = 0; i != Max; ++i)
		res += latches_[i].ToString();
	return res;
}

std::string LatchManager::ToString() const
{
	std::string res;
	for (int i = 0; i != Hash; ++i) {
		auto str = latch_set_[i].ToString();
		if (str != "") res += str + "\n";
	}
	return res;
}

} // namespace Mushroom
