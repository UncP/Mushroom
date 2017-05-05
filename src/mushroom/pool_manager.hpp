/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:09:33
**/

#ifndef _POOL_MANAGER_HPP_
#define _POOL_MANAGER_HPP_

#include "utility.hpp"

namespace Mushroom {

class PoolManager
{
	public:
		static void SetManagerInfo(uint32_t page_size, uint32_t pool_size, uint32_t hash_bits,
			uint32_t seg_bits);

		PoolManager();

		~PoolManager();

		void Reset();

		page_t TotalPage() const { return cur_; }

		inline bool ReachMaxPool() { return tot_ >= (PoolSize - 1); }

		Page* GetPage(page_t page_no);
		Page* NewPage(uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree);

		bool Free();

		PoolManager(const PoolManager &) = delete;
		PoolManager& operator=(const PoolManager &) = delete;

	private:
		static uint32_t HashMask;
		static uint32_t PoolSize;

		void Link(uint16_t hash, uint16_t victim);

		page_t     cur_;
		uint16_t   tot_;
		HashEntry *entries_;
		PagePool  *pool_;
};

} // namespace Mushroom

#endif /* _POOL_MANAGER_HPP_ */