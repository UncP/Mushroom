/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-18 10:10:03
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#include <sstream>
#include <atomic>
// #include <shared_mutex>
#include <cassert>
#include <pthread.h>

#include "status.hpp"

namespace Mushroom {

class BTreePage;

class Latch
{
	public:
		Latch():id_(0x7FFFFFFF) { assert(pthread_rwlock_init(&mutex_, NULL) == 0); }

		page_id Id() const { return id_; }
		void SetId(page_id id) { id_ = id; }
		void Pin()   { ++users_; }
		void UnPin() { --users_; }
		bool Free() const { return users_.load(std::memory_order_relaxed) == 0; }

		void LockShared() {
			// mutex_.lock_shared();
			assert(pthread_rwlock_rdlock(&mutex_) == 0);
		}

		void UnlockShared() {
			// mutex_.unlock_shared();
			assert(pthread_rwlock_unlock(&mutex_) == 0);
			UnPin();
		}

		void Lock() {
			// mutex_.lock();
			assert(pthread_rwlock_wrlock(&mutex_) == 0);
		}

		void Unlock() {
			// mutex_.unlock();
			assert(pthread_rwlock_unlock(&mutex_) == 0);
			UnPin();
		}

		void Upgrade() {
			// mutex_.unlock_shared();
			// mutex_.lock();
			assert(pthread_rwlock_unlock(&mutex_) == 0);
			assert(pthread_rwlock_wrlock(&mutex_) == 0);
		}

		void Downgrade() {
			// mutex_.unlock();
			// mutex_.lock_shared();
			assert(pthread_rwlock_unlock(&mutex_) == 0);
			assert(pthread_rwlock_rdlock(&mutex_) == 0);
		}

		std::string ToString() const {
			if (id_ == 0x7FFFFFFF) return std::string();
			std::ostringstream os;
			os << id_ << ": " << users_ << std::endl;
			return std::move(os.str());
		}

		BTreePage *page_;

	private:
		std::atomic<int>        users_;
		page_id                 id_;
		// std::shared_timed_mutex mutex_;
		pthread_rwlock_t        mutex_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */