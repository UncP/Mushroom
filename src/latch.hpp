/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-18 10:10:03
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#include <cassert>
#include <pthread.h>

#include "status.hpp"

namespace Mushroom {

class BTreePage;
class LatchManager;

class SpinLatch
{
	public:
		SpinLatch():mutex_(0), exclusive_(0), pending_(0), share_(0) { }

		void SpinReadLock() {
			uint32_t prev;
			for (;;) {
				if (__sync_lock_test_and_set(&mutex_, 1))
					continue;
				if ((prev = !(exclusive_ | pending_)))
					++share_;
				__sync_lock_release(&mutex_);
				if (prev) return ;
				sched_yield();
			}
		}

		void SpinWriteLock() {
			uint32_t prev;
			for (;;) {
				if (__sync_lock_test_and_set(&mutex_, 1))
					continue;
				if ((prev = !(share_ | exclusive_)))
					exclusive_ = 1, pending_ = 0;
				else
					pending_ = 1;
				__sync_lock_release(&mutex_);
				if (prev) return;
				sched_yield();
			}
		}

		uint32_t SpinWriteTry() {
			uint32_t prev;
			if (__sync_lock_test_and_set(&mutex_, 1))
				return 0;
			if ((prev = !(exclusive_ | share_)))
				exclusive_ = 1;
			__sync_lock_release(&mutex_);
			return prev;
		}

		void SpinReleaseWrite() {
			while (__sync_lock_test_and_set(&mutex_, 1))
				sched_yield();
			exclusive_ = 0;
			__sync_lock_release(&mutex_);
		}

		void SpinReleaseRead() {
			while (__sync_lock_test_and_set(&mutex_, 1))
				sched_yield();
			--share_;
			__sync_lock_release(&mutex_);
		}

	private:
		volatile uint8_t  mutex_;
		volatile uint8_t  exclusive_;
		volatile uint8_t  pending_;
		volatile uint16_t share_;
};

class Latch
{
	friend class LatchManager;
	public:
		Latch():pin_(0), prev_(0), next_(0), hash_(0), page_no_(0xFFFFFFFF) {
			assert(pthread_rwlock_init(lock_, 0) == 0);
		}

		void Pin() { __sync_fetch_and_add(&pin_, 1); }

		void Unpin() { __sync_fetch_and_add(&pin_, -1); }

		void LockShared() {
			pthread_rwlock_rdlock(lock_);
		}
		void Lock() {
			pthread_rwlock_wrlock(lock_);
		}
		void UnlockShared() {
			pthread_rwlock_unlock(lock_);
			Unpin();
		}
		void Unlock() {
			pthread_rwlock_unlock(lock_);
			Unpin();
		}

		void Upgrade() {
			pthread_rwlock_unlock(lock_);
			pthread_rwlock_wrlock(lock_);
		}

		~Latch() {
			assert(pthread_rwlock_destroy(lock_) == 0);
		}

		BTreePage *page_;

	private:
		volatile uint16_t pin_;
		volatile uint16_t prev_;
		volatile uint16_t next_;
		volatile uint16_t hash_;
		volatile page_id  page_no_;
		SpinLatch         busy_;
		pthread_rwlock_t  lock_[1];
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */