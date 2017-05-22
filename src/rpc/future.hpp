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

		Future(uint32_t id, const Func &cb1):id_(id), status_(Pending), cb1_(cb1), cb2_(0) { }

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
			cb1_();
			mutex_.Lock();
			if (status_ == Pending)
				status_ = Ok;
			cond_.Signal();
			mutex_.Unlock();
			if (status_ != TimeOut && cb2_)
				cb2_();
		}

		inline void Cancel() {
			mutex_.Lock();
			if (status_ == Pending)
				status_ = TimeOut;
			cond_.Signal();
			mutex_.Unlock();
		}

		inline void OnCallback(const Func &cb2) {
			cb2_ = cb2;
		}

	private:
		uint32_t id_;
		uint8_t  status_;
		Mutex    mutex_;
		Cond     cond_;
		Func     cb1_;
		Func     cb2_;
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

		inline void Clear() { futures_.clear(); }

	private:
		std::vector<Future *> futures_;
};

} // namespace Mushroom

#endif /* _FUTURE_HPP_ */