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

namespace Mushroom {

class BTreePage
{
	friend
		std::ostream& operator<<(std::ostream& os, const BTreePage *bpage) {
			return os << bpage->ToString();
		}

	public:
		static enum { ROOT = 0, BRANCH, LEAF } TYPE;

		static const int PageSize     = 4096;
		static const int DataIdLength = 6;

		std::string ToString() const;

	private:
		uint32_t  page_no_;
		uint32_t  right_;
		uint16_t  total_key_;
		uint16_t  total_child_;
		uint8_t   type_;
		char      data_[0];
};

class BTreePageBucket
{
	public:

		BTreePageBucket& operator=(const BTreePageBucket &) = delete;
		BTreePageBucket(const BTreePageBucket &) = delete;

		~BTreePageBucket() {
			if (pages_) delete [] pages_;
			pages_ = nullptr;
		}

	private:

		BTreePage **pages_;
		int len_;
		int max_;
};

class BTreePager
{
	public:

		BTreePager& operator=(const BTreePager &) = delete;
		BTreePager(const BTreePager &) = delete;

		~BTreePager() {
			if (bucket_) delete [] bucket_;
			bucket_ = nullptr;
		}

	private:
		uint32_t hash_;
		BTreePageBucket *bucket_;
};

} // namespace Mushroom

#endif /* _BT_PAGE_HPP_ */