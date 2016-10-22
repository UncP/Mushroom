/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 09:09:54
**/

#ifndef _BTREE_PAGE_HPP_
#define _BTREE_PAGE_HPP_

#include <cassert>

#include "status.hpp"
#include "slice.hpp"

namespace Mushroom {

class BTreePage
{
	public:
		static enum { ROOT = 0, BRANCH, LEAF } TYPE;

		static const uint16_t PageSize  = 4096;

		static const uint16_t IndexByte = 2;
		static const uint16_t PageByte  = sizeof(page_id);

		static BTreePage* NewPage(page_id page_no, int type, uint8_t key_len, uint8_t level);

		Status Write(const int fd);

		Status Read(const page_id page_no, const int fd);

		void Reset(page_id page_no, int type, uint8_t key_len, uint8_t level) {
			assert(!dirty_ && !occupy_);
			memset(this, 0, PageSize);
			page_no_ = page_no;
			type_    = type;
			key_len_ = key_len;
			level_   = level;
		}

		int Type() const { return type_; }

		void AssignType(int type) { type_ = type; }

		const char* Data() const { return data_; }
		uint8_t KeyLen() const { return key_len_; }
		uint8_t Level() const { return level_; }
		page_id PageNo() const { return page_no_; }
		page_id First() const { return first_; }
		page_id Next() const {
			KeySlice *key = (KeySlice *)(data_ + Index()[total_key_-1]);
			return key->PageNo();
		}
		bool Dirty() const { return dirty_; }
		bool Occupy() const { return occupy_; }
		uint16_t KeyNo() const { return total_key_; }

		void AssignPageNo(page_id page_no) { page_no_ = page_no; }
		void AssignFirst(page_id first) {
			assert(type_ != LEAF);
			first_ = first;
		}
		void SetOccupy(bool o) {
			if (o) assert(!occupy_);
			else assert(occupy_);
			occupy_ = o;
		}

		page_id Descend(const KeySlice *key) const;

		bool Insert(const KeySlice *key);

		bool Search(KeySlice *key) const;

		bool Ascend(KeySlice *key, page_id *page_no, uint16_t *index);

		void Split(BTreePage *that, KeySlice *slice = nullptr);

		std::string ToString() const;

	private:
		uint16_t* Index() const {
			return (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
		}
		KeySlice* Key(const uint16_t *index, int pos) const {
			return (KeySlice *)(data_ + index[pos]);
		}
		bool Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type = Eq) const;

		static enum { Eq, Ge } TraverseType;

		page_id  page_no_;
		page_id  first_;
		uint16_t total_key_;
		uint8_t  key_len_;
		uint8_t  level_;
		unsigned    type_:2;
		unsigned   dirty_:1;
		unsigned  occupy_:1;
		unsigned    lock_:1;
		unsigned readers_:3;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _BTREE_PAGE_HPP_ */