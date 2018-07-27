/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-3-25 23:21:00
**/

#ifndef _BARRIER_HPP_
#define _BARRIER_HPP_

#include "../include/mutex.hpp"
#include "../include/cond.hpp"

namespace Mushroom {

class Barrier
{
	public:
		Barrier(int num):num_(num), cnt_(num_), gen_(0) { }

		void Wait() {
			mutex_.Lock();
			uint32_t gen = gen_;
			if (--cnt_ == 0) {
				++gen_;
				cnt_ = num_;
				mutex_.Unlock();
				cond_.Broadcast();
			}
			while (gen == gen_)
				cond_.Wait(mutex_);
			mutex_.Unlock();
		}

	private:
		Mutex          mutex_;
		Cond           cond_;
		const uint32_t num_;
		uint32_t       cnt_;
		uint32_t       gen_;
};

} // namespace Mushroom

#endif /* _BARRIER_HPP_ */