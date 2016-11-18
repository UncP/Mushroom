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

#include <cassert>
#include <iostream>
#include <shared_mutex>

#include "status.hpp"

namespace Mushroom {

class SharedLock
{
	public:

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
			UnlockShared();
			Lock();
		}

		void Downgrade() {
			Unlock();
			LockShared();
		}

	private:

		std::shared_timed_mutex mutex_;
};

} // namespace Mushroom

#endif /* _SHARED_LOCK_HPP_ */