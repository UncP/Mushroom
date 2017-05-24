/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-22 09:09:54
**/

#ifndef _BTREE_PAGE_HPP_
#define _BTREE_PAGE_HPP_

#include "../include/utility.hpp"
#include "slice.hpp"

namespace Mushroom {

typedef enum { InsertOk, ExistedKey, MoveRight } InsertStatus;

class KeySlice;
class BLinkTree;

class Page : private NoCopy
{
	friend class BLinkTree;
	public:
		static uint32_t PageSize;

		static enum { ROOT = 1, BRANCH = 2, LEAF = 4 } Type;

		static const uint32_t PageByte  = sizeof(page_t);
		static const uint32_t IndexByte = 2;

		static void SetPageInfo(uint32_t page_size);

		static uint16_t CalculateDegree(uint8_t key_len, uint8_t pre_len = 0);

		void InsertInfiniteKey();

		Page(page_t id, uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree);

		inline page_t Next() const {
			KeySlice *key = (KeySlice *)(data_ + Index()[total_key_-1]);
			return key->page_no_;
		}

		void AssignFirst(page_t first);

		page_t Descend(const KeySlice *key) const;

		bool Search(KeySlice *key, uint16_t *index) const;

		InsertStatus Insert(const KeySlice *key, page_t &page_no);

		void Insert(Page *that, KeySlice *key);

		bool Ascend(KeySlice *key, page_t *page_no, uint16_t *index);

		void Split(Page *that, KeySlice *slice);

		bool NeedSplit();

	private:

		inline uint16_t* Index() const {
			return (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
		}

		inline KeySlice* Key(const uint16_t *index, int pos) const {
			return (KeySlice *)(data_ + index[pos]);
		}

		bool Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type = 1) const;

		page_t   page_no_;
		page_t   first_;
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