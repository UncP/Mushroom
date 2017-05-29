/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-11-18 10:10:03
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#include "../include/utility.hpp"
#include "../include/spin_lock.hpp"
#include "../include/atomic.hpp"

namespace Mushroom {

class LatchManager;

class Latch : private NoCopy
{
	friend class LatchManager;
	public:
		Latch() {
			Reset();
			assert(!pthread_rwlock_init(lock_, 0));
		}

		inline void Reset() {
			pin_  = 0;
			hash_ = 0;
			prev_ = 0;
			next_ = 0;
			id_   = ~page_t(0);
		}

		inline void Pin() {
			++pin_;
		}

		inline void LockShared() {
			pthread_rwlock_rdlock(lock_);
		}

		inline void Lock() {
			pthread_rwlock_wrlock(lock_);
		}

		inline void UnlockShared() {
			pthread_rwlock_unlock(lock_);
			Unpin();
		}

		inline void Unlock() {
			pthread_rwlock_unlock(lock_);
			Unpin();
		}

		inline bool TryWriteLock() {
			return !pthread_rwlock_trywrlock(lock_);
		}

		inline void Upgrade() {
			pthread_rwlock_unlock(lock_);
			pthread_rwlock_wrlock(lock_);
		}

		~Latch() {
			assert(!pthread_rwlock_destroy(lock_));
		}

	private:
		inline void Unpin() {
			--pin_;
		}

		atomic_16_t       pin_;
		uint16_t          hash_;
		uint16_t          prev_;
		uint16_t          next_;
		page_t            id_;
		SpinLock          busy_;
		pthread_rwlock_t  lock_[1];
};

class HashEntry {
	public:
		HashEntry():slot_(0) { }

		inline void Lock() {
			lock_.Lock();
		}

		inline bool TryLock() {
			return lock_.TryLock();
		}

		inline void Unlock() {
			lock_.Unlock();
		}

		inline void SetSlot(uint16_t slot) {
			slot_ = slot;
		}

		inline uint16_t GetSlot() const {
			return slot_;
		}

	private:
		SpinLock  lock_;
		uint16_t  slot_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */