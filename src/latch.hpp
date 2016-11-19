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

		void Pin();

		void UnPin();

		bool Busy() const;

		void LockShared();

		void UnlockShared();

		void Lock();

		void Unlock();

		void Upgrade();

		void Downgrade();

		std::string ToString() const {
			std::ostringstream os;
			// os << id_ << ": " << (pin_ == true ? "true " : "false ") << users_ << std::endl;
			os << id_ << ": " << users_ << std::endl;
			return std::move(os.str());
		}

	private:
		// std::atomic<bool> pin_;
		std::atomic<int>  users_;
		page_id           id_;
		SharedLock        shared_lock_;
};

} // namespace Mushroom

#endif /* _LATCH_HPP_ */