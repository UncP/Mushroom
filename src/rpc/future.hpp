/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-10 20:22:22
**/

#ifndef _FUTURE_HPP_
#define _FUTURE_HPP_

#include <vector>
#include <functional>

#include "../include/utility.hpp"
#include "../include/mutex.hpp"
#include "../include/cond.hpp"

namespace Mushroom {

template<typename T>
class Future : private NoCopyTemplate<T>
{
	public:
		typedef std::function<void(Future<T> *)> CallBack;

		enum Status { Pending = 0x0, Ok = 0x1, Timeout = 0x2 };

		Future():cb_(0) { }

		Future(Func &&cb):cb_(cb) { }

		inline T& Value() { return value_; }

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

		inline void Notify(Marshaller &mar) {
			mar >> value_;
			mutex_.Lock();
			if (status_ == Pending) {
				status_ = Ok;
				cond_.Signal();
			}
			mutex_.Unlock();
			if (cb_) cb_(this);
		}

		inline void Cancel() {
			mutex_.Lock();
			if (status_ == Pending) {
				status_ = Timeout;
				cond_.Signal();
			}
			mutex_.Unlock();
		}

	private:
		uint8_t   status_;
		T         value_;
		Mutex     mutex_;
		Cond      cond_;
		CallBack  cb_;
};

template<typename T>
class FutureGroup : private NoCopyTemplate<T>
{
	public:
		FutureGroup(int size) { futures_.reserve(size); }

		inline void Add(Future<T> *future) { futures_.push_back(future); }

		inline void Wait() {
			for (auto e : futures_)
				e->Wait();
		}

		inline void Cancel() {
			for (auto e : futures_)
				e->Cancel();
		}

		inline Future<T>* operator[](uint32_t i) {
			return futures_[i];
		}

	private:
		std::vector<Future<T> *> futures_;
};

} // namespace Mushroom

#endif /* _FUTURE_HPP_ */