/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-10 20:22:22
**/

#ifndef _FUTURE_HPP_
#define _FUTURE_HPP_

#include <vector>

#include "../include/utility.hpp"
#include "../include/mutex.hpp"
#include "../include/cond.hpp"

namespace Mushroom {

template<typename T>
class Future : private NoCopyTemplate<T>
{
	public:
		enum Status { Pending = 0x0, Ok = 0x1, Timeout = 0x2 };

		Future():status_(Pending), cb_(0) { }

		inline void SetId(uint32_t id) { id_ = id; }

		inline uint32_t GetId() const { return id_; }

		inline void OnCallback(Func &&cb) { cb_ = cb; }

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
			} else {
				mutex_.Unlock();
				return ;
			}
			if (cb_) cb_();
			mutex_.Unlock();
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
		uint8_t  status_;
		uint32_t id_;
		T        value_;
		Mutex    mutex_;
		Cond     cond_;
		Func     cb_;
};

} // namespace Mushroom

#endif /* _FUTURE_HPP_ */