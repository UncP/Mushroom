/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-25 12:56:16
**/

#ifndef _PAGE_POOL_HPP_
#define _PAGE_POOL_HPP_

#include "utility.hpp"
#include "page.hpp"

namespace Mushroom {

class PagePool
{
	friend class PoolManager;

	public:
		static void SetPoolInfo(uint8_t seg_bits) {
			SegBits = seg_bits;
			SegSize = 1 << seg_bits;
			SegMask = SegSize - 1;
		}

		PagePool():base_(0), mem_(0), prev_(0), next_(0) { }

		void Initialize(page_id page_no) {
			base_ = page_no & ~SegMask;
			mem_ = new char[Page::PageSize * SegSize];
		}

		Page* GetPage(page_id page_no) {
			return (Page *)(mem_ + Page::PageSize * (page_no & SegMask));
		}

		PagePool(const PagePool &) = delete;
		PagePool& operator=(const PagePool &) = delete;

	private:
		static uint32_t SegBits;
		static uint32_t SegSize;
		static uint32_t SegMask;

		page_id   base_;
		char     *mem_;
		PagePool *prev_;
		PagePool *next_;
};

} // namespace Mushroom

#endif /* _PAGE_POOL_HPP_ */