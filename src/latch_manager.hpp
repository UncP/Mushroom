/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-21 16:50:18
**/

#ifndef _LATCH_MANAGER_HPP_
#define _LATCH_MANAGER_HPP_

#include <string>
#include <atomic>
#include <pthread.h>

#include "latch.hpp"
#include "btree_page.hpp"

namespace Mushroom {

class LatchManager
{
	public:
		struct LatchSet {
			LatchSet():slot_(0) { assert(pthread_rwlock_init(lock_, 0) == 0); }
			~LatchSet() { assert(pthread_rwlock_destroy(lock_) == 0); }
			pthread_rwlock_t  lock_[1];
			uint32_t slot_;
		};

		LatchManager();

		Latch *GetLatch(page_id page_no);

		~LatchManager() {
			delete [] latch_set_;
			delete [] latches_;
		}

		friend std::ostream& operator<<(std::ostream &os, const LatchManager *lm) {

		}
	private:
		void Link(uint32_t idx, uint32_t cur, page_id id);

		static const int total = 128;
		static const int mask  = 37;

		std::atomic<uint32_t> deployed_;
		std::atomic<uint32_t> next_;
		LatchSet        *latch_set_;
		Latch           *latches_;
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */