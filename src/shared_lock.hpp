/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 10:01:42
**/

#ifndef _SHARED_LOCK_HPP_
#define _SHARED_LOCK_HPP_

#include <mutex>
#include <condition_variable>
#include <string>

#include "utility.hpp"

namespace Mushroom {

class SharedLock
{
	public:

		using lock_id = page_id;

		SharedLock():shared_count_(0), block_shared_(false), exclusive_(false) { }

		void SetId(lock_id id) { id_ = id; }

		void LockShared();

		void UnlockShared();

		void Lock();

		void Unlock();

		void Upgrade();

		std::string ToString() {
			char buf[32];
			snprintf(buf, 32, "%d %s %s", shared_count_, block_shared_ ? "true" : "false",
				exclusive_ ? "true" : "false");
			return std::string(buf);
		}

	private:

		lock_id                 id_;
		uint32_t                shared_count_;
		bool                    block_shared_;
		bool                    exclusive_;
		std::mutex              mutex_;
		std::condition_variable shared_condition_;
		std::condition_variable exclusive_condition_;
};

} // namespace Mushroom

#endif /* _SHARED_LOCK_HPP_ */