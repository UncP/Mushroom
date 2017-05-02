/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-18 10:10:03
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#ifdef __APPLE__

#include <cerrno>
#include <atomic>
#include <thread>

#ifndef PTHREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_SHARED 1
#endif
#ifndef PTHREAD_PROCESS_PRIVATE
#define PTHREAD_PROCESS_PRIVATE 2
#endif

#ifndef UNUSED
#define UNUSED(expr) (void)(expr)
#endif  // UNUSED

typedef std::atomic_flag pthread_spinlock_t;

static inline int pthread_spin_destroy(pthread_spinlock_t *lock) {
  UNUSED(lock);
  return 0;
}

static inline int pthread_spin_lock(pthread_spinlock_t *lock) {
  while (lock->test_and_set(std::memory_order_acquire))
    std::this_thread::yield();
  return 0;
}

static inline int pthread_spin_trylock(pthread_spinlock_t *lock) {
  if (lock->test_and_set(std::memory_order_acquire))
    return 0;
  else
    return EBUSY;
}

static inline int pthread_spin_unlock(pthread_spinlock_t *lock) {
  lock->clear(std::memory_order_release);
  return 0;
}

static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  UNUSED(pshared);
  pthread_spin_unlock(lock);
  return 0;
}

#endif  // __APPLE__

#include <cassert>
#include <pthread.h>

#include "utility.hpp"

namespace Mushroom {

class ConditionVariable;

class Mutex
{
	friend class ConditionVariable;
	public:
		Mutex() {
			assert(!pthread_mutex_init(mutex_, 0));
		}

		void Lock() {
			pthread_mutex_lock(mutex_);
		}

		bool TryLock() {
			return !pthread_mutex_trylock(mutex_);
		}

		void Unlock() {
			pthread_mutex_unlock(mutex_);
		}

		~Mutex() {
			assert(!pthread_mutex_destroy(mutex_));
		}

	private:
		pthread_mutex_t mutex_[1];
};

class ConditionVariable
{
	public:
		ConditionVariable() {
			assert(!pthread_cond_init(cond_, 0));
		}

		void Wait(Mutex *mutex) {
			pthread_cond_wait(cond_, mutex->mutex_);
		}

		void Signal() {
			pthread_cond_signal(cond_);
		}

		~ConditionVariable() {
			assert(!pthread_cond_destroy(cond_));
		}

	private:
		pthread_cond_t   cond_[1];
};

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
		Latch() {
			Reset();
			assert(!pthread_rwlock_init(lock_, 0));
		}

		void Reset() {
			pin_  = 0;
			hash_ = 0;
			prev_ = 0;
			next_ = 0;
			page_no_ = ~page_t(0);
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
		page_t            page_no_;
		SpinLatch         busy_;
		pthread_rwlock_t  lock_[1];
};

class HashEntry {
	public:
		HashEntry():slot_(0) { }
		SpinLatch latch_;
		uint16_t  slot_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */