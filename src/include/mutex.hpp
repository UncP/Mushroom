/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-05 22:53:02
**/

#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#include <pthread.h>
#include <cassert>

#include "utility.hpp"

namespace Mushroom {

class Cond;

class Mutex : private NoCopy
{
	friend class Cond;
	public:
		Mutex() {
			assert(!pthread_mutex_init(mutex_, 0));
		}

		inline void Lock() {
			pthread_mutex_lock(mutex_);
		}

		inline void Unlock() {
			pthread_mutex_unlock(mutex_);
		}

		~Mutex() {
			assert(!pthread_mutex_destroy(mutex_));
		}

	private:
		pthread_mutex_t mutex_[1];
};

} // namespace Mushroom

#endif /* _MUTEX_HPP_ */