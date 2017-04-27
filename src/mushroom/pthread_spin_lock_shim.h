#ifndef PTHREAD_SPIN_LOCK_SHIM
#define PTHREAD_SPIN_LOCK_SHIM

// I am not sure is there any more options
#ifdef __APPLE__

#include <cerrno>

#include <atomic>
#include <thread>  // NOLINT(build/c++11) for std::this_thread::yield();

// workaround for pshared
// ref: https://linux.die.net/man/3/pthread_spin_init
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
  // do nothing
  return 0;
}

static inline int pthread_spin_lock(pthread_spinlock_t *lock) {
  while (lock->test_and_set(std::memory_order_acquire)) {
    // wait
    std::this_thread::yield();
  }
  return 0;
}

static inline int pthread_spin_trylock(pthread_spinlock_t *lock) {
  if (lock->test_and_set(std::memory_order_acquire)) {
    return 0;
  } else {
    return EBUSY;
  }
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

#endif
