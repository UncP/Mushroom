/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-11-18 10:10:03
**/

#ifndef _LATCH_HPP_
#define _LATCH_HPP_

#include <sstream>
#include <atomic>

#include "status.hpp"
#include "shared_lock.hpp"

namespace Mushroom {

class Latch
{
	public:
		Latch():id_(0x7FFFFFFF) { }

		void SetId(page_id id) { id_ = id; }

		page_id Id() const { return id_; }

		void Pin() { pin_ = true, ++users_; }

		void UnPin() {
			if (!--users_)
				pin_ = false;
		}

		bool Busy() const { return pin_.load(std::memory_order_relaxed); }

		void LockShared() { shared_lock_.LockShared(); }

		void UnlockShared() { shared_lock_.UnlockShared(); }

		void Lock() { shared_lock_.Lock(); }

		void Unlock() { shared_lock_.Unlock(); }

		void Upgrade() { shared_lock_.Upgrade(); }

		void Downgrade() { shared_lock_.Downgrade(); }

		std::string ToString() const {
			std::ostringstream os;
			os << id_ << ": " << (pin_ == true ? "true " : "false ") << users_ << std::endl;
			return std::move(os.str());
		}

	private:
		std::atomic<bool> pin_;
		std::atomic<int>  users_;
		page_id           id_;
		SharedLock        shared_lock_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */