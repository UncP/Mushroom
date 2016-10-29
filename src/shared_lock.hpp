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

#include <pthread.h>
#include <cassert>

#include "status.hpp"

namespace Mushroom {

class SharedLock
{
	public:

		using lock_id = page_id;

		SharedLock():prev_(nullptr), next_(nullptr) {
			assert(!pthread_rwlock_init(&mutex_, NULL));
		}

		page_id Id() const { return id_; }

		void SetId(page_id id) { id_ = id; }

		void LockShared() {
			// mutex_.lock_shared();
			pthread_rwlock_rdlock(&mutex_);
		}

		void UnlockShared() {
			// mutex_.unlock_shared();
			pthread_rwlock_unlock(&mutex_);
		}

		void Lock() {
			// mutex_.lock();
			pthread_rwlock_wrlock(&mutex_);
		}

		void Unlock() {
			// mutex_.unlock();
			pthread_rwlock_unlock(&mutex_);
		}

		void Upgrade() {
			// mutex_.unlock_shared();
			// mutex_.lock();
			pthread_rwlock_unlock(&mutex_);
			pthread_rwlock_wrlock(&mutex_);
		}

		void Downgrade() {
			// mutex_.unlock();
			// mutex_.lock_shared();
			pthread_rwlock_unlock(&mutex_);
			pthread_rwlock_rdlock(&mutex_);
		}

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

		~SharedLock() {
			assert(!pthread_rwlock_destroy(&mutex_));
		}

	private:

		lock_id                 id_;
		// std::shared_timed_mutex mutex_;
		pthread_rwlock_t       mutex_;
		SharedLock             *prev_;
		SharedLock             *next_;
};

} // namespace Mushroom

#endif /* _SHARED_LOCK_HPP_ */