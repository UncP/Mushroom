/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-21 16:50:18
**/

#ifndef _LATCH_MANAGER_HPP_
#define _LATCH_MANAGER_HPP_

#include <mutex>
#include <string>

#include "latch.hpp"
#include "btree_page.hpp"

namespace Mushroom {

class LatchSet
{
	public:
		LatchSet() { }

		Latch* GetLatch(page_id page_no);

		std::string ToString() const;

	private:
		static const int Max = 8;

		std::mutex mutex_;
		Latch      latches_[Max];
};

class LatchManager
{
	public:
		LatchManager() { }

		Latch *GetLatch(page_id page_no);

		std::string ToString() const;

	private:
		static const int Hash = 64;
		static const int Mask = Hash - 1;

		LatchSet    latch_set_[Hash];
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */