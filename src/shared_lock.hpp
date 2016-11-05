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
#include <atomic>
#include <iostream>

#include "status.hpp"

namespace Mushroom {

class SharedLock
{
	public:

		using lock_id = page_id;

		SharedLock():id_(0xFFFFFFFF), prev_(nullptr), next_(nullptr), users_(0), occupy_(false) {
			assert(!pthread_rwlock_init(&mutex_, NULL));
		}

		lock_id Id() const { return id_; }

		bool Occupy() const { return occupy_.load(std::memory_order_relaxed); }

		void SetId(page_id id) {
			occupy_ = true;
			id_ = id;
		}

		void SetOccupy(bool occupy) { occupy_ = occupy; }

		void LockShared() {
			// mutex_.lock_shared();
			++users_;
			assert(!pthread_rwlock_rdlock(&mutex_));
		}

		void UnlockShared() {
			// mutex_.unlock_shared();
			assert(!pthread_rwlock_unlock(&mutex_));
			--users_;
			if (!users_)
				occupy_ = false;
		}

		int Users() const { return users_.load(std::memory_order_relaxed); }

		void Lock() {
			// mutex_.lock();
			++users_;
			assert(!pthread_rwlock_wrlock(&mutex_));
		}

		void Unlock() {
			// mutex_.unlock();
			assert(!pthread_rwlock_unlock(&mutex_));
			--users_;
			occupy_ = false;
		}

		void Upgrade() {
			// mutex_.unlock_shared();
			// mutex_.lock();
			// assert(!pthread_rwlock_unlock(&mutex_));
			// assert(!pthread_rwlock_wrlock(&mutex_));
			assert(!pthread_rwlock_unlock(&mutex_));
			assert(!pthread_rwlock_wrlock(&mutex_));
			// UnlockShared();
			// Lock();
		}

		void Downgrade() {
			// mutex_.unlock();
			// mutex_.lock_shared();
			// assert(!pthread_rwlock_unlock(&mutex_));
			// assert(!pthread_rwlock_rdlock(&mutex_));
			assert(!pthread_rwlock_unlock(&mutex_));
			assert(!pthread_rwlock_rdlock(&mutex_));
			// Unlock();
			// LockShared();
		}

		SharedLock* Prev() const { return prev_; }
		SharedLock* Next() const { return next_; }

		void Link(SharedLock *that) {
			next_ = that;
			if (that)
				that->prev_ = this;
		}

		void Detach() {
			prev_->next_ = next_;
			prev_ = nullptr;
			next_ = nullptr;
			id_   = 0;
		}

		~SharedLock() {
			assert(!users_);
			assert(!pthread_rwlock_destroy(&mutex_));
		}

	private:

		lock_id                 id_;
		// std::shared_timed_mutex mutex_;
		pthread_rwlock_t        mutex_;
		SharedLock             *prev_;
		SharedLock             *next_;
		std::atomic<int>        users_;
		std::atomic<bool>       occupy_;
};

} // namespace Mushroom

#endif /* _SHARED_LOCK_HPP_ */