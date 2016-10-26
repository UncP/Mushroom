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

		SharedLock():prev_(nullptr), next_(nullptr) { }

		page_id Id() const { return id_; }

		void SetId(page_id id) { id_ = id; }

		void LockShared() { mutex_.lock_shared(); }

		void UnlockShared() { mutex_.unlock_shared(); }

		void Lock() { mutex_.lock(); }

		void Unlock() { mutex_.unlock(); }

		void Upgrade() { mutex_.unlock_shared(); mutex_.lock(); }

		void Downgrade() { mutex_.unlock(); mutex_.lock_shared(); }

		SharedLock* Prev() const { return prev_; }
		SharedLock* Next() const { return next_; }

		void Link(SharedLock *prev, SharedLock *next) {
			prev_ = prev;
			next_ = next;
		}

		void Detach() {
			prev_->next_ = next_;
			prev_ = nullptr;
			next_ = nullptr;
			id_   = 0;
		}

	private:

		lock_id                 id_;
		std::shared_timed_mutex mutex_;
		SharedLock             *prev_;
		SharedLock             *next_;
};

} // namespace Mushroom

#endif /* _SHARED_LOCK_HPP_ */