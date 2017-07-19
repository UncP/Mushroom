/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-07-19 16:55:10
**/

#include "../include/spin_lock.hpp"

namespace Mushroom {

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

