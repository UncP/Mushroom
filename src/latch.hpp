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
			pthread_rwlock_rdlock(&mutex_);
		}

		void UnlockShared() {
			// mutex_.unlock_shared();
			pthread_rwlock_unlock(&mutex_);
			UnPin();
		}

		void Lock() {
			// mutex_.lock();
			pthread_rwlock_wrlock(&mutex_);
		}

		void Unlock() {
			// mutex_.unlock();
			pthread_rwlock_unlock(&mutex_);
			UnPin();
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

		std::string ToString() const {
			if (id_ == 0x7FFFFFFF) return std::string();
			std::ostringstream os;
			os << id_ << ": " << users_ << std::endl;
			return os.str();
		}

		~Latch() { assert(pthread_rwlock_destroy(&mutex_) == 0); }

		BTreePage *page_;

	private:
		std::atomic<int>        users_;
		page_id                 id_;
		// std::shared_timed_mutex mutex_;
		pthread_rwlock_t        mutex_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */