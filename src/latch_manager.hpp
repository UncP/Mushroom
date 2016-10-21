/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-21 16:50:18
**/

#ifndef _LATCH_MANAGER_HPP_
#define _LATCH_MANAGER_HPP_

#include <shared_mutex>

#include "utility.hpp"

namespace Mushroom {

class Lock
{
	public:

		using lock_id = page_id;

		Lock() { }

		void ReadLock() { mutex_.lock_shared(); }

		void ReadUnLock() { mutex_.unlock_shared(); }

		void WriteLock() { mutex_.lock(); }

		void WriteUnlock() { mutex_.unlock(); }

	private:

		lock_id                 id_;
		std::shared_timed_mutex mutex_;
};

class LatchManager
{
	public:

	private:

};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */