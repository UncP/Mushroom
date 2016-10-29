/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-29 15:51:51
**/

#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#include <pthread.h>
#include <cassert>

class Mutex
{
	public:
		Mutex() {
			assert(!pthread_mutex_init(&mutex_, NULL));
		}

		void Lock() {
			pthread_mutex_lock(&mutex_);
		}

		void Unlock() {
			pthread_mutex_unlock(&mutex_);
		}

		~Mutex() {
			assert(!pthread_mutex_destroy(&mutex_));
		}

	private:
		pthread_mutex_t mutex_;
};

#endif /* _MUTEX_HPP_ */