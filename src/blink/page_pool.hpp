/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-03-25 12:56:16
**/

#ifndef _PAGE_POOL_HPP_
#define _PAGE_POOL_HPP_

#include <cstring>

#include "../include/utility.hpp"
#include "page.hpp"

namespace Mushroom {

class PoolManager;

class PagePool : private NoCopy
{
	friend class PoolManager;

	public:
		static void SetPoolInfo(uint32_t seg_bits) {
			SegBits = seg_bits;
			SegSize = 1 << seg_bits;
			SegMask = SegSize - 1;
		}

		PagePool() { }

		inline void Initialize(page_t base) {
			base_ = base;
			mem_ = new char[Page::PageSize << SegBits];
			memset(mem_, 0, Page::PageSize << SegBits);
		}

		inline Page* GetPage(page_t page_no) {
			return (Page *)(mem_ + Page::PageSize * (page_no & SegMask));
		}

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