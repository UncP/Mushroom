/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-21 16:50:18
**/

#ifndef _LATCH_MANAGER_HPP_
#define _LATCH_MANAGER_HPP_

#include <mutex>
#include <condition_variable>
#include <string>

#include "shared_lock.hpp"
#include "latch.hpp"

namespace Mushroom {

class LatchSet
{
	public:

		LatchSet() { }

		Latch* GetLatch(page_id page_no);

		std::string ToString() const;

	private:
		static const int Max = 4;

		SharedLock lock_;
		Latch      latches_[Max];
};

class LatchManager
{
	public:

		LatchManager() { }

		Latch *GetLatch(page_id page_no);

		std::string ToString() const;

	private:
		static const int Hash = 8;
		static const int Mask = Hash - 1;

		LatchSet    latch_set_[Hash];
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */