/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-05 22:53:02
**/

#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#include <pthread.h>
#include <cassert>

namespace Mushroom {

class Cond;

class Mutex
{
	friend class Cond;
	public:
		Mutex() {
			assert(!pthread_mutex_init(mutex_, 0));
		}

		inline void Lock() {
			pthread_mutex_lock(mutex_);
		}

		inline bool TryLock() {
			return !pthread_mutex_trylock(mutex_);
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