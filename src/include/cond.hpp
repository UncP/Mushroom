/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-05 22:54:44
**/

#ifndef _COND_HPP_
#define _COND_HPP_

#include <sys/time.h>
#include <cerrno>

#include "utility.hpp"
#include "mutex.hpp"

namespace Mushroom {

class Cond : private NoCopy
{
	public:
		Cond() {
			assert(!pthread_cond_init(cond_, 0));
		}

		inline void Wait(Mutex &mutex) {
			pthread_cond_wait(cond_, mutex.mutex_);
		}

		inline void Signal() {
			pthread_cond_signal(cond_);
		}

		inline void Broadcast() {
			pthread_cond_broadcast(cond_);
		}

		inline bool TimedWait(Mutex &mutex, int64_t millisecond) {
			struct timeval tv;
			gettimeofday(&tv, 0);
			timespec abstime;
			abstime.tv_sec  = tv.tv_sec;
			int64_t nsec = int64_t(tv.tv_usec) * 1000 + millisecond * 1000000;
			if (nsec >= 1000000000) {
				++abstime.tv_sec;
				nsec -= 1000000000;
			}
			abstime.tv_nsec = nsec;
			return pthread_cond_timedwait(cond_, mutex.mutex_, &abstime) == ETIMEDOUT;
		}

		~Cond() {
			assert(!pthread_cond_destroy(cond_));
		}

	private:
		pthread_cond_t cond_[1];
};

} // namespace Mushroom

#endif /* _COND_HPP_ */