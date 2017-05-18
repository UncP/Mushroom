/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-10 20:22:22
**/

#ifndef _FUTURE_HPP_
#define _FUTURE_HPP_

#include <vector>

#include "../include/utility.hpp"
#include "../include/mutex.hpp"
#include "../include/cond.hpp"

namespace Mushroom {

class Future : private NoCopy
{
	public:
		enum Status { Pending = 0x0, Ok = 0x1, TimeOut = 0x2 };

		Future(uint32_t id, const Func &callback):id_(id), status_(Pending), callback_(callback) { }

		uint32_t id() const { return id_; }

		inline void Wait() {
			mutex_.Lock();
			while (status_ == Pending)
				cond_.Wait(mutex_);
			mutex_.Unlock();
		}

		inline bool ok() {
			mutex_.Lock();
			bool ret = (status_ == Ok);
			mutex_.Unlock();
			return ret;
		}

		inline void Notify() {
			callback_();
			mutex_.Lock();
			status_ = Ok;
			mutex_.Unlock();
			cond_.Signal();
		}

		inline void Cancel() {
			mutex_.Lock();
			if (status_ == Pending)
				status_ = TimeOut;
			mutex_.Unlock();
			cond_.Signal();
		}

	private:
		uint32_t  id_;
		uint8_t   status_;
		Cond      cond_;
		Mutex     mutex_;
		Func      callback_;
};

class FutureGroup : private NoCopy
{
	public:
		FutureGroup(int size) { futures_.reserve(size); }

		inline void Add(Future *future) { futures_.push_back(future); }

		inline void Wait() {
			for (auto e : futures_)
				e->Wait();
		}

		inline void Cancel() {
			for (auto e : futures_)
				e->Cancel();
		}

		inline Future* operator[](uint32_t i) {
			return futures_[i];
		}

	private:
		std::vector<Future *> futures_;
};

} // namespace Mushroom

#endif /* _FUTURE_HPP_ */