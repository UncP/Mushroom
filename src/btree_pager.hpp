/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 17:28:04
**/

#ifndef _BTREE_PAGER_HPP_
#define _BTREE_PAGER_HPP_

#include <string>
#include <mutex>

#include "mutex.hpp"
#include "status.hpp"
#include "btree_page.hpp"

namespace Mushroom {

class BTreePageBucket
{
	public:
		BTreePageBucket():len_(0) { memset(ages_, 0, Max * sizeof(uint16_t)); }

		BTreePage* GetPage(const page_id page_no, const int fd);

		Status PinPage(BTreePage *page, const int fd);

		BTreePage* GetEmptyPage(page_id page_no, int type, uint8_t key_len, uint8_t level, int fd);

		Status Clear(const int fd);

		int Length() const { return len_; }

		std::string ToString() const;

		BTreePageBucket& operator=(const BTreePageBucket &) = delete;
		BTreePageBucket(const BTreePageBucket &) = delete;

	private:

		static const int Max = 8;

		// std::mutex mutex_;
		// Mutex      mutex_;
		BTreePage *pages_[Max];
		uint32_t   ages_[Max];
		int        len_;
};

class BTreePager
{
	public:
		BTreePager(int fd):fd_(fd), curr_(0) { }

		std::string ToString() const;

		BTreePage* GetPage(const page_id page_no);

		BTreePage* NewPage(int type, uint8_t key_len, uint8_t level, bool pin = true);

		Status PinPage(BTreePage *page);

		Status Close();

		int fd() const { return fd_; }

		BTreePager& operator=(const BTreePager &) = delete;
		BTreePager(const BTreePager &) = delete;

	private:
		static const uint32_t Hash = 1024;
		static const page_id  Mask = Hash - 1;

		// std::mutex      mutex_;
		// Mutex           mutex_;
		int             fd_;
		page_id         curr_;
		BTreePageBucket bucket_[Hash];
};

} // namespace Mushroom

#endif /* _BTREE_PAGER_HPP_ */