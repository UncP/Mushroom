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

#include <mutex>

#include "shared_lock.hpp"

namespace Mushroom {

class LatchSet
{

	public:

		LatchSet():head_(nullptr) { }


	private:
		std::mutex mutex_;
		Lock      *head_;
};

class LatchManager
{
	public:

		LatchManager() { }

	private:
		static const int Hash = 16;
		static const int Mask = Hash - 1;

		LatchSet set_[Hash];
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */