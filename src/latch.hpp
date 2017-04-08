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

#include "utility.hpp"

namespace Mushroom {

#ifndef NOLATCH

class SpinLatch
{
	public:
		SpinLatch() {
			assert(!pthread_spin_init(lock_, 0));
		}

		void Lock() {
			pthread_spin_lock(lock_);
		}

		bool TryLock() {
			return !pthread_spin_trylock(lock_);
		}

		void Unlock() {
			pthread_spin_unlock(lock_);
		}

		~SpinLatch() {
			assert(!pthread_spin_destroy(lock_));
		}

	private:
		pthread_spinlock_t lock_[1];
};

class Latch
{
	friend class LatchManager;
	public:
		Latch():pin_(0), hash_(0), prev_(0), next_(0), page_no_(~page_id(0)) {
			assert(!pthread_rwlock_init(lock_, 0));
		}

		void Pin() { __sync_fetch_and_add(&pin_, 1); }

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

		bool TryWriteLock() {
			return !pthread_rwlock_trywrlock(lock_);
		}

		void Upgrade() {
			pthread_rwlock_unlock(lock_);
			pthread_rwlock_wrlock(lock_);
		}

		~Latch() {
			assert(!pthread_rwlock_destroy(lock_));
		}

	private:
		void Unpin() { __sync_fetch_and_add(&pin_, -1); }

		volatile uint16_t pin_;
		uint16_t          hash_;
		uint16_t          prev_;
		uint16_t          next_;
		page_id           page_no_;
		SpinLatch         busy_;
		pthread_rwlock_t  lock_[1];
};

#endif

class HashEntry {
	public:
		HashEntry():slot_(0) { }
		#ifndef NOLATCH
		SpinLatch latch_;
		#endif
		uint16_t  slot_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */