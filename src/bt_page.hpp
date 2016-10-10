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

typedef uint32_t page_id;

class BTreePage
{
	friend
		std::ostream& operator<<(std::ostream& os, const BTreePage *bpage) {
			return os << bpage->ToString();
		}

	public:
		static enum { ROOT = 0, BRANCH, LEAF } TYPE;

		static const int PageSize  = 4096;
		static const int DataId    = 4;

		static const int IndexByte = 2;
		static const int PageByte  = 4;


		static BTreePage* NewPage(const page_id page_no);

		Status Write(const int fd);

		Status Read(const int fd);

		int Type() const { return type_; }

		page_id PageNo() const { return page_no_; }

		bool Dirty() const { return dirty_; }

		bool Occupy() const { return occupy_; }

		page_id Descend(const Slice &key) const;

		void Info(uint8_t key_len, uint16_t max) const;

		std::string ToString() const;

	private:
		page_id  page_no_;
		page_id  right_;
		uint16_t total_key_;
		uint16_t total_child_;
		unsigned   type_:2;
		unsigned  dirty_:1;
		unsigned occupy_:1;
		unsigned        :4;
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
		BTreePager(int fd):fd_(fd), total_(0) { }

		std::string ToString() const;

		BTreePage* GetPage(const page_id page_no);

		Status Close();

		BTreePager& operator=(const BTreePager &) = delete;
		BTreePager(const BTreePager &) = delete;

	private:
		static const uint32_t Hash = 32;
		static const page_id  Mask = (page_id)Hash - 1;

		int             fd_;
		page_id         total_;
		BTreePageBucket bucket_[Hash];
};

} // namespace Mushroom

#endif /* _BT_PAGE_HPP_ */