/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:09:33
**/

#ifndef _POOL_MANAGER_HPP_
#define _POOL_MANAGER_HPP_

#include "latch.hpp"
#include "btree_page.hpp"
#include "page_pool.hpp"

namespace Mushroom {

class PoolManager
{
	public:
		PoolManager(int fd, page_id *cur);

		~PoolManager();

		static void SetPoolManagerInfo(uint32_t page_size, uint32_t pool_size, uint8_t hash_bits,
			uint8_t seg_bits);

		page_id Total() const { return *cur_; }

		BTreePage* GetPage(page_id page_no);
		BTreePage* NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree);

		bool Free();

		PoolManager(const PoolManager &) = delete;
		PoolManager(const PoolManager &&) = delete;
		PoolManager& operator=(const PoolManager &) = delete;
		PoolManager& operator=(const PoolManager &&) = delete;

		static const uint32_t LatchPages = 4;

	private:
		static uint32_t PoolSize;
		static uint32_t HashMask;

		void Link(uint16_t hash, uint16_t victim);

		int        fd_;
		page_id   *cur_;
		uint16_t   tot_;
		HashEntry *entries_;
		PagePool  *pool_;
};

} // namespace Mushroom

#endif /* _POOL_MANAGER_HPP_ */