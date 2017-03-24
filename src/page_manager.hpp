/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:09:33
**/

#ifndef _PAGE_MANAGER_HPP_
#define _PAGE_MANAGER_HPP_

#include "latch.hpp"
#include "btree_page.hpp"

namespace Mushroom {

class PageManager
{
	public:
		PageManager(int fd, page_id *cur);
		~PageManager();

		page_id Total() const { return *cur_; }

		BTreePage* GetPage(page_id page_no);
		BTreePage* NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree);

		bool Free();

		PageManager(const PageManager &) = delete;
		PageManager(const PageManager &&) = delete;
		PageManager& operator=(const PageManager &) = delete;
		PageManager& operator=(const PageManager &&) = delete;

		static const uint32_t LatchPages = 4;

		static const uint32_t SegBits  = 4;
		static const uint32_t SegSize  = 16;
		static const uint32_t PoolMask = 15;
		static const uint32_t PoolSize = 4800;
		static const uint32_t HashMask = 1023;

	private:
		struct PagePool {
			PagePool():pin_(0), base_(0), mem_(0), prev_(0), next_(0) { }

			void Initialize(page_id page_no);

			BTreePage* GetPage(page_id page_no);

			uint16_t  pin_;
			page_id   base_;
			char     *mem_;
			PagePool *prev_;
			PagePool *next_;
		};

		int        fd_;
		page_id   *cur_;
		uint16_t   tot_;
		HashEntry *entries_;
		PagePool  *pool_;
};

} // namespace Mushroom

#endif /* _PAGE_MANAGER_HPP_ */