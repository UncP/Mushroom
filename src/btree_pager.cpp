/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 17:34:51
**/

#include <cassert>
#include <unistd.h>
#include <sstream>

#include "btree_pager.hpp"
#include "utility.hpp"

namespace Mushroom {

std::string BTreePageBucket::ToString() const
{
	if (!len_) return std::string("\n");
	std::ostringstream os;
	os << "len: " << len_ << "\n";
	for (int i = 0; i != len_; ++i)
		os << ages_[i] << " ";
	os << "\n";
	return os.str();
}

std::string BTreePager::ToString() const
{
	std::ostringstream os;
	for (int i = 0; i != Hash; ++i) {
		if (!bucket_[i].Length()) continue;
		os << i << " " << bucket_[i].ToString();
	}
	return os.str();
}

BTreePage* BTreePageBucket::GetPage(const page_id page_no, const int fd)
{
	std::lock_guard<std::mutex> lock(mutex_);

	int index = len_;
	uint32_t fresh = 0xFFFFFFFF;
	for (int i = 0; i != len_; ++i) {
		if (page_no == pages_[i]->PageNo()) {
			++ages_[i];
			return pages_[i];
		}
		if (ages_[i] < fresh && !pages_[i]->Occupy()) {
			index = i;
			fresh = ages_[i];
		}
	}
	assert(index != len_);
	if (len_ != Max) {
		BTreePage *page = BTreePage::NewPage(page_no, 0, 0, 0);
		if (page) {
			assert(page->Read(page_no, fd));
			pages_[index] = page;
			++len_;
			return page;
		}
	}
	assert(pages_[index]->Write(fd));
	ages_[index] = 0;
	assert(pages_[index]->Read(page_no, fd));

	return pages_[index];
}

BTreePage* BTreePageBucket::GetEmptyPage(page_id page_no, int type, uint8_t key_len,
	uint8_t level, int fd)
{
	assert(0);
	std::lock_guard<std::mutex> lock(mutex_);

	int index = len_;
	uint32_t fresh = 0xFFFFFFFF;
	for (int i = 0; i != len_; ++i) {
		if (ages_[i] < fresh && !pages_[i]->Occupy()) {
			index = i;
			fresh = ages_[i];
		}
	}
	assert(index != len_);
	assert(pages_[index]->Write(fd));
	ages_[index] = 0;
	pages_[index]->Reset(page_no, type, key_len, level);
	return pages_[index];
}

Status BTreePageBucket::PinPage(BTreePage *page, const int fd)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (len_ < Max) {
		ages_[len_] = 0;
		pages_[len_++] = page;
		return Success;
	}
	int index = len_;
	uint32_t fresh = 0xFFFFFFFF;
	for (int i = 0; i != len_; ++i) {
		if (ages_[i] < fresh && !pages_[i]->Occupy()) {
			index = i;
			fresh = ages_[i];
		}
	}
	assert(index != len_);
	assert(pages_[index]->Write(fd));
	ages_[index]  = 0;
	pages_[index] = page;
	return Success;
}

BTreePage* BTreePager::GetPage(const page_id page_no)
{
	assert(page_no < curr_);
	BTreePageBucket &bucket = bucket_[page_no & Mask];
	BTreePage *page = bucket.GetPage(page_no, fd_);
	return page;
}

BTreePage* BTreePager::NewPage(int type, uint8_t key_len, uint8_t level, bool pin)
{
	std::lock_guard<std::mutex> lock(mutex_);

	BTreePageBucket &bucket = bucket_[curr_ & Mask];
	BTreePage *page = BTreePage::NewPage(curr_, type, key_len, level);
	if (!pin) {
		assert(page);
		++curr_;
		return page;
	}

	if (page)
		assert(bucket.PinPage(page, fd_));
	else
		page = bucket.GetEmptyPage(curr_, type, key_len, level, fd_);
	++curr_;
	return page;
}

Status BTreePager::PinPage(BTreePage *page)
{
	BTreePageBucket &bucket = bucket_[page->PageNo() & Mask];
	assert(bucket.PinPage(page, fd_));
	return Success;
}

Status BTreePageBucket::Clear(const int fd)
{
	for (int i = len_ - 1; i >= 0; --i) {
		// assert(pages_[i]->Write(fd));
		delete [] pages_[i];
		ages_[i] = 0;
		--len_;
	}
	return Success;
}

Status BTreePager::Close()
{
	for (int i = 0; i != Hash; ++i)
		assert(bucket_[i].Clear(fd_));
	if (fd_ > 0)
		close(fd_);
	fd_ = -1;
	return Success;
}

} // namespace Mushroom
