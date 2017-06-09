/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-22 09:09:54
**/

#ifndef _BTREE_PAGE_HPP_
#define _BTREE_PAGE_HPP_

#include <string>
#include <cassert>

#include "../include/utility.hpp"
#include "slice.hpp"

namespace Mushroom {

typedef enum { InsertOk = 0x0, ExistedKey = 0x1, MoveRight = 0x2 } InsertStatus;

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

		static Page* NewPage(uint8_t key_len);

		static void DeletePage(Page *page);

		Page(page_t id, uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree);

		inline page_t Next() const {
			KeySlice *key = (KeySlice *)(data_ + Index()[total_key_-1]);
			return key->page_no_;
		}

		inline void SetPageNo(page_t page_no) { page_no_ = page_no; }

		inline page_t PageNo() const { return page_no_; }

		inline bool Dirty() const { return dirty_; }

		inline void Clean() { dirty_ = 0; }

		void AssignFirst(page_t first);

		page_t Descend(const KeySlice *key) const;

		bool Search(const KeySlice *key, uint16_t *index) const;

		InsertStatus Insert(const KeySlice *key, page_t &page_no);

		bool Insert(const KeySlice *key);

		void Insert(Page *that, KeySlice *key);

		bool Update(const KeySlice *old_key, const KeySlice *new_key, page_t &page_no);

		void Split(Page *that, KeySlice *slice);

		void Expand(uint8_t to);

		bool Move(Page *that, KeySlice *old_key, KeySlice *new_key);

		void Combine(Page *left, Page *right, KeySlice *old_key, KeySlice *new_key,KeySlice *slice);

		bool Full() const;

		bool NeedSplit();

		std::string ToString(bool f, bool f2) const;

		bool FenceKeyEqual(const Page *that) const;

		bool FenceKeyLessEqual(const Page *that) const;

	private:
		bool Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type = 1) const;

		void FillFrom(uint16_t above);

		inline uint16_t* Index() const {
			return (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
		}

		inline KeySlice* Key(const uint16_t *index, uint16_t pos) const {
			return (KeySlice *)(data_ + index[pos]);
		}

		page_t   page_no_;
		page_t   first_;
		uint16_t total_key_;
		uint16_t degree_;
		uint8_t  dirty_;
		uint8_t  type_;
		uint8_t  level_;
		uint8_t  key_len_;
		uint8_t  pre_len_;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _BTREE_PAGE_HPP_ */