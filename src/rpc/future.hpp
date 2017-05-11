/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-10 20:22:22
**/

#ifndef _FUTURE_HPP_
#define _FUTURE_HPP_

#include "../include/utility.hpp"
#include "../include/atomic.hpp"

namespace Mushroom {

class Future : private NoCopy
{
	public:
		enum Status { Pending = 0x0, Ok = 0x1, TimeOut = 0x2, Bad = 0x3 };

		Future(uint32_t id, const Func &callback)
		:id_(id), status_(Pending), time_out_(0), callback_(callback) { }

		uint32_t id() const { return id_; }

		inline void Wait() {
			while (status_.get() == Pending && !time_out_.get())
				sched_yield();
			if (time_out_.get())
				status_ = time_out_;
		}

		inline bool ok() { return status_.get() == Ok; }

		inline void Notify() { callback_(); status_ = Ok; }

		inline void Abandon() { time_out_ = 1; }

	private:
		uint32_t   id_;
		atomic_8_t status_;
		atomic_8_t time_out_;
		Func       callback_;
};

} // namespace Mushroom

#endif /* _FUTURE_HPP_ */