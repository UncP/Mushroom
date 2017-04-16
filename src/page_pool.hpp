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
		static void SetPoolInfo(uint32_t seg_bits) {
			SegBits = seg_bits;
			SegSize = 1 << seg_bits;
			SegMask = SegSize - 1;
		}

		PagePool():mem_(0) { Reset(); }

		inline void Reset() {
			base_ = 0;
			next_ = 0;
			if (mem_) memset(mem_, 0, (Page::PageSize << SegBits));
		}

		inline void Initialize(page_t page_no) {
			base_ = page_no & ~SegMask;
			if (!mem_) mem_ = new char[Page::PageSize << SegBits];
		}

		inline Page* GetPage(page_t page_no) {
			return (Page *)(mem_ + Page::PageSize * (page_no & SegMask));
		}

		PagePool(const PagePool &) = delete;
		PagePool& operator=(const PagePool &) = delete;

		static uint32_t SegSize;

	private:
		static uint32_t SegBits;
		static uint32_t SegMask;

		page_t    base_;
		char     *mem_;
		PagePool *next_;
};

} // namespace Mushroom

#endif /* _PAGE_POOL_HPP_ */