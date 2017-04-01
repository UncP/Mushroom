/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-22 09:09:54
**/

#ifndef _BTREE_PAGE_HPP_
#define _BTREE_PAGE_HPP_

#include <cassert>
#include <string>

#include "utility.hpp"
#include "slice.hpp"

namespace Mushroom {

typedef enum { InsertOk, ExistedKey, MoveRight } InsertStatus;

class Page
{
	friend class BLinkTree;
	public:

		static uint32_t PageSize;

		static enum { ROOT = 0, BRANCH, LEAF } TYPE;
		static const uint32_t PageByte  = sizeof(page_id);
		static const uint32_t IndexByte = 2;

		static void SetPageInfo(uint32_t page_size);

		static uint16_t CalculateDegree(uint8_t key_len, uint8_t pre_len = 0);

		void Initialize(page_id page_no, int type, uint8_t key_len, uint8_t level, uint16_t degree);

		page_id Next() const {
			KeySlice *key = (KeySlice *)(data_ + Index()[total_key_-1]);
			return key->PageNo();
		}

		void AssignFirst(page_id first) {
			assert(type_ != LEAF);
			first_ = first;
		}

		page_id Descend(const KeySlice *key) const;

		bool Search(KeySlice *key, uint16_t *index) const;

		InsertStatus Insert(const KeySlice *key, page_id &page_no);

		void Insert(Page *that, KeySlice *key);

		bool Ascend(KeySlice *key, page_id *page_no, uint16_t *index);

		void Split(Page *that, KeySlice *slice);

		bool NeedSplit();

		std::string ToString() const;

	private:
		uint16_t* Index() const {
			return (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
		}
		KeySlice* Key(const uint16_t *index, int pos) const {
			return (KeySlice *)(data_ + index[pos]);
		}
		bool Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type = 1) const;

		page_id  page_no_;
		page_id  first_;
		uint16_t total_key_;
		uint16_t degree_;
		uint8_t  type_;
		uint8_t  level_;
		uint8_t  key_len_;
		uint8_t  pre_len_;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _BTREE_PAGE_HPP_ */