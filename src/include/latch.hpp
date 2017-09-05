/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-09-05 16:03:53
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#include <pthread.h>
#include <cassert>

#include "utility.hpp"

namespace Mushroom {

class Latch : private NoCopy {
	public:
		Latch() { }

		void Init() { assert(!pthread_rwlock_init(latch_, 0)); }

		inline void ReadLock() {
			pthread_rwlock_rdlock(latch_);
		}

		inline void WriteLock() {
			pthread_rwlock_wrlock(latch_);
		}

		inline void Unlock() {
			pthread_rwlock_unlock(latch_);
		}

	private:
		pthread_rwlock_t latch_[1];
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */