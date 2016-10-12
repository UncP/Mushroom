/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 17:28:04
**/

#ifndef _BT_PAGE_HPP_
#define _BT_PAGE_HPP_

#include <cstdint>
#include <iostream>
#include <string>
#include <memory.h>

#include "status.hpp"
#include "slice.hpp"

namespace Mushroom {

class BTreePage
{
	friend
		std::ostream& operator<<(std::ostream& os, const BTreePage *bpage) {
			return os << bpage->ToString();
		}

	public:
		static enum { ROOT = 0, BRANCH, LEAF } TYPE;

		static const uint16_t PageSize  = 4096;

		static const uint16_t IndexByte = 2;
		static const uint16_t PageByte  = sizeof(page_id);
		static const uint16_t DataId    = sizeof(page_id);

		static BTreePage* NewPage(const page_id page_no, int type, uint8_t key_len);

		Status Write(const int fd);

		Status Read(const int fd);

		int Type() const { return type_; }

		void AssignType(int type) { type_ = type; }

		const char* Data() const { return data_; }
		page_id PageNo() const { return page_no_; }
		page_id Right() const { return right_; }
		bool Dirty() const { return dirty_; }
		bool Occupy() const { return occupy_; }
		uint16_t KeyNo() const { return total_key_; }
		uint16_t ChildNo() const { return total_child_; }

		void AssignPageNo(page_id page_no) { page_no_ = page_no; }
		void AssignRight(page_id right) { right_ = right; }
		void AssignFirst(page_id first) {
			assert(type_ != LEAF);
			first_ = first;
		}

		page_id Descend(const Slice &key) const;

		bool Insert(const Slice &key);
		bool Insert(const KeySlice *key);

		void Split(BTreePage *that, char *key = nullptr);

		void Info(uint8_t key_len) const;

		std::string ToString() const;

	private:

		void Compact();

		page_id  page_no_;
		page_id  right_;
		page_id  first_;
		uint16_t total_key_;
		uint16_t total_child_;
		uint8_t  key_len_;
		unsigned    type_:2;
		unsigned   dirty_:1;
		unsigned  occupy_:1;
		unsigned    lock_:1;
		unsigned readers_:3;
		char     data_[0];
};

class BTreePageBucket
{
	friend
		std::ostream& operator<<(std::ostream &os, const BTreePageBucket &bucket) {
			return os << bucket.ToString();
		}

	public:
		BTreePageBucket():len_(0) { memset(ages_, 0, Max * sizeof(uint16_t)); }

		BTreePage* GetPage(const page_id page_no, const int fd);

		Status PinPage(BTreePage *page, const int fd);

		Status Close(const int fd);

		int Length() const { return len_; }

		std::string ToString() const;

		BTreePageBucket& operator=(const BTreePageBucket &) = delete;
		BTreePageBucket(const BTreePageBucket &) = delete;

	private:

		static const int Max = 8;

		BTreePage *pages_[Max];
		uint16_t   ages_[Max];
		int        len_;
};

class BTreePager
{
	friend
		std::ostream& operator<<(std::ostream &os, const BTreePager &pager) {
			return os << pager.ToString();
		}

	public:
		BTreePager(int fd):fd_(fd), curr_(1) { }

		std::string ToString() const;

		BTreePage* GetPage(const page_id page_no);

		BTreePage* NewPage(int type, uint8_t key_len);

		Status Close();

		BTreePager& operator=(const BTreePager &) = delete;
		BTreePager(const BTreePager &) = delete;

	private:
		static const uint32_t Hash = 32;
		static const page_id  Mask = (page_id)Hash - 1;

		int             fd_;
		page_id         curr_;
		BTreePageBucket bucket_[Hash];
};

} // namespace Mushroom

#endif /* _BT_PAGE_HPP_ */