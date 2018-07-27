/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-03-19 13:09:33
**/

#ifndef _POOL_MANAGER_HPP_
#define _POOL_MANAGER_HPP_

#include "../include/utility.hpp"
#include "../include/atomic.hpp"

namespace Mushroom {

class Page;
class HashEntry;
class PagePool;

class PoolManager : private NoCopy
{
	public:
		static void SetManagerInfo(uint32_t page_size, uint32_t pool_size, uint32_t hash_bits,
			uint32_t seg_bits);

		PoolManager();

		~PoolManager();

		Page* GetPage(page_t page_no);

		Page* NewPage(uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree);

		void Free();

	private:
		static uint32_t HashMask;
		static uint32_t PoolSize;

		void Link(uint16_t hash, uint16_t victim);

		Atomic<page_t> cur_page_;
		atomic_16_t    total_pool_;
		HashEntry     *entries_;
		PagePool      *pool_;
};

} // namespace Mushroom

#endif /* _POOL_MANAGER_HPP_ */