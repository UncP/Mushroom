/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-04 09:18:22
**/

#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <pthread.h>

namespace Mushroom {

template<typename T>
class Thread
{
	public:
		Thread(void* (*func)(void *), T *args):func_(func), args_(args) { }

		inline bool Start() {
			return !pthread_create(&id_, 0, func_, args_);
		}

		inline bool Stop() {
			return !pthread_join(id_, 0);
		}

		Thread(const Thread &) = delete;
		Thread& operator=(const Thread &) = delete;

	private:
		void*   (*func_)(void *);
		T        *args_;
		pthread_t id_;
};

} // namespace Mushroom

#endif /* _THREAD_HPP_ */