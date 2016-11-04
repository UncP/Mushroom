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

#include "shared_lock.hpp"

namespace Mushroom {

class LatchSet
{
	public:

		LatchSet();

		SharedLock* GetLock(page_id page_no);

	private:
		static const int Max = 8;

		std::mutex mutex_;
		std::condition_variable has_free_;
		SharedLock locks_[Max];
		SharedLock *busy_ = nullptr;
		SharedLock *free_ = nullptr;
};

class LatchManager
{
	public:

		LatchManager() { }

		void LockShared(page_id page_no);

		void UnlockShared(page_id page_no);

		void Lock(page_id page_no);

		void Unlock(page_id page_no);

		void Upgrade(page_id page_no);

		void Downgrade(page_id page_no);

	private:
		static const int Hash = 8;
		static const int Mask = Hash - 1;

		LatchSet    latch_set_[Hash];
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */