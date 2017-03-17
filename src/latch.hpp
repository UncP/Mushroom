/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-18 10:10:03
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#include <atomic>
#include <cassert>
#include <pthread.h>

#include "status.hpp"

namespace Mushroom {

class BTreePage;
class LatchManager;

class Latch
{
	friend class LatchManager;
	public:
		Latch():pin_(0), prev_(0), next_(0), id_(0xFFFFFFFF) {
			assert(pthread_rwlock_init(busy_, 0) == 0);
			assert(pthread_rwlock_init(lock_, 0) == 0);
		}

		void LockShared() {
			pthread_rwlock_rdlock(lock_);
		}
		void Lock() {
			pthread_rwlock_wrlock(lock_);
		}
		void UnlockShared() {
			pthread_rwlock_unlock(lock_);
			--pin_;
		}
		void Unlock() {
			pthread_rwlock_unlock(lock_);
			--pin_;
		}

		void Upgrade() {
			pthread_rwlock_unlock(lock_);
			pthread_rwlock_wrlock(lock_);
		}

		~Latch() {
			assert(pthread_rwlock_destroy(busy_) == 0);
			assert(pthread_rwlock_destroy(lock_) == 0);
		}

		BTreePage *page_;

	private:
		std::atomic<uint32_t> pin_;

		uint32_t          prev_;
		uint32_t          next_;
		uint32_t          hash_;
		page_id           id_;
		pthread_rwlock_t  busy_[1];
		pthread_rwlock_t  lock_[1];
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */