/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-05 22:56:41
**/

#ifndef _SPIN_LOCK_HPP_
#define _SPIN_LOCK_HPP_

#include <pthread.h>
#include <cassert>

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
#endif

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

#endif  /* __APPLE__ */

namespace Mushroom {

class SpinLock
{
	public:
		SpinLock() {
			assert(!pthread_spin_init(lock_, 0));
		}

		inline void Lock() {
			pthread_spin_lock(lock_);
		}

		inline bool TryLock() {
			return !pthread_spin_trylock(lock_);
		}

		inline void Unlock() {
			pthread_spin_unlock(lock_);
		}

		~SpinLock() {
			assert(!pthread_spin_destroy(lock_));
		}

	private:
		pthread_spinlock_t lock_[1];
};

} // namespace Mushroom

#endif /* _SPIN_LOCK_HPP_ */