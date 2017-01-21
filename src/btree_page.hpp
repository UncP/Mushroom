/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-22 09:09:54
**/

#ifndef _BTREE_PAGE_HPP_
#define _BTREE_PAGE_HPP_

#include <cassert>

#include "status.hpp"
#include "slice.hpp"

namespace Mushroom {

typedef enum { InsertOk, ExistedKey , MoveRight, NeedExpand } InsertStatus;

class BTreePage
{
	public:
		static enum { ROOT = 0, BRANCH, LEAF } TYPE;

		static const uint16_t PageSize  = 4096;

		static const uint16_t PageByte  = sizeof(page_id);
		static const uint16_t IndexByte = 2;

		static BTreePage* NewPage(page_id page_no, int type, uint8_t key_len, uint8_t level,
			uint16_t degree);

		static uint16_t CalculateDegree(uint8_t key_len, uint8_t pre_len = 0);

		void Reset(page_id page_no, int type, uint8_t key_len, uint8_t level, uint16_t degree);

		Status Read(const page_id page_no, const int fd);

		Status Write(const int);

		page_id PageNo() const { return page_no_; }
		page_id First() const { return first_; }
		uint8_t KeyLen() const { return key_len_; }
		uint8_t Level() const { return level_; }
		uint16_t Degree() const { return degree_; }
		int Type() const { return type_; }
		const char* Data() const { return data_; }
		page_id Next() const {
			KeySlice *key = (KeySlice *)(data_ + Index()[total_key_-1]);
			return key->PageNo();
		}

		void AssignType(int type) { type_ = type; }
		void AssignPageNo(page_id page_no) { page_no_ = page_no; }
		void AssignFirst(page_id first) {
			assert(type_ != LEAF);
			first_ = first;
		}

		page_id Descend(const KeySlice *key) const;

		bool Search(KeySlice *key) const;

		InsertStatus Insert(const KeySlice *key, page_id &page_no);

		void Insert(BTreePage *that, KeySlice *key);

		bool Ascend(KeySlice *key, page_id *page_no, uint16_t *index);

		void Split(BTreePage *that, KeySlice *slice);

		bool NeedSplit();

		void Analyze() const;

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
		uint8_t  dirty_;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _BTREE_PAGE_HPP_ */