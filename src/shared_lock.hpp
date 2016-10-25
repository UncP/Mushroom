/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 10:01:42
**/

#ifndef _SHARED_LOCK_HPP_
#define _SHARED_LOCK_HPP_

#include <shared_mutex>

#include "status.hpp"

namespace Mushroom {

class SharedLock
{
	public:

		using lock_id = page_id;

		SharedLock() { }

		void LockShared() {
			mutex_.lock_shared();
		}

		void UnlockShared() {
			mutex_.unlock_shared();
		}

		void Lock() {
			mutex_.lock();
		}

		void Unlock() {
			mutex_.unlock();
		}

		void Upgrade() {
			mutex_.unlock_shared();
			mutex_.lock();
		}

		void Downgrade() {
			mutex_.unlock();
			mutex_.lock_shared();
		}

	private:

		lock_id           id_;
		std::shared_timed_mutex mutex_;
};

} // namespace Mushroom

#endif /* _SHARED_LOCK_HPP_ */