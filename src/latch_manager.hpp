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

#include "shared_lock.hpp"

namespace Mushroom {

class LatchSet
{
	public:

		LatchSet():head_(nullptr) { }

		SharedLock* FindLock(page_id page_no);

		void PinLock(SharedLock *lk);

		void UnpinLock(SharedLock *lk);

	private:
		std::mutex  mutex_;
		SharedLock *head_;
};

class LatchManager
{
	public:

		LatchManager();

		void LockShared(page_id page_no);

		void UnlockShared(page_id page_no);

		void Lock(page_id page_no);

		void Unlock(page_id page_no);

		void Upgrade(page_id page_no);

		void Downgrade(page_id page_no);

		~LatchManager() {
			if (free_)
				delete [] free_;
			free_ = nullptr;
		}

	private:

		SharedLock* AllocateFree(page_id id);

		static const int Max  = 16;
		static const int Hash = 8;
		static const int Mask = Hash - 1;

		std::mutex  mutex_;

		SharedLock *free_;

		LatchSet    latch_set_[Hash];
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */