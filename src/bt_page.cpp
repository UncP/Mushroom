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

#include "bt_page.hpp"

namespace Mushroom {

#define KeyPtr(data, idx, key_len, data_id) (data + (idx * (key_len + data_id)))

std::string BTreePage::ToString() const
{
	std::string str("type: ");
	if (type_ == LEAF)   str += "leaf  ";
	if (type_ == BRANCH) str += "branch  ";
	if (type_ == ROOT)   str += "root  ";
	char no[16];
	snprintf(no, 16, "%d  ", page_no_);
	str += "page_no: " + std::string(no);
	snprintf(no, 16, "%d  ", total_key_);
	str += "tot_key: " + std::string(no);
	snprintf(no, 16, "%d  ", total_child_);
	str += "tot_chd: " + std::string(no);
	snprintf(no, 16, "%d  ", right_);
	str += "right: " + std::string(no);
	if (dirty_)
		str += "dirty: true\n";
	else
		str += "dirty: false\n";
	return std::move(str);
}

std::string BTreePageBucket::ToString() const
{
	if (!len_) return std::string("\n");
	std::string str("len: ");
	char no[16];
	snprintf(no, 16, "%d\n", len_);
	str += std::string(no);
	for (int i = 0; i != len_; ++i)
		str += pages_[i]->ToString();
	return std::move(str);
}

std::string BTreePager::ToString() const
{
	std::string str;
	char no[16];
	for (int i = 0; i != Hash; ++i) {
		if (!bucket_[i].Length()) continue;
		snprintf(no, 16, "%2d ", i);
		str += std::string(no);
		str += bucket_[i].ToString();
	}
	return std::move(str);
}

page_id BTreePage::Descend(const Slice &key) const
{

	return 0;
}

void BTreePage::Info(const uint8_t key_len, const uint16_t max) const
{
	std::string str = ToString();
	if (!total_key_) {
		std::cout << str;
		return ;
	}

	assert(total_key_ < max);
	for (uint16_t i = 0; i != total_key_; ++i) {
		uint16_t idx = *(uint16_t *)(data_ + (i << 1));
		char no[8];
		snprintf(no, 8, "%d ", idx);
		str += std::string(no);
	}
	str += "\n";

	const char *keys = data_ + (max << 1);
	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)KeyPtr(keys, i, key_len, DataId);
		assert(!key->DataEmpty());
		str += std::string(key->Key(), key_len) + " ";
	}
	std::cout << str << std::endl;
}

BTreePage* BTreePage::NewPage(const page_id page_no)
{
	BTreePage *page = (BTreePage *)calloc(1, BTreePage::PageSize);
	assert(page);
	page->page_no_ = page_no;
	return page;
}

Status BTreePage::Write(const int fd)
{
	if (dirty_) {
		dirty_ = 0;
		if (pwrite(fd, &page_no_, PageSize, page_no_ * PageSize) != PageSize)
			return Fail;
	}
	return Success;
}

Status BTreePage::Read(const int fd)
{
	if (pread(fd, &page_no_, PageSize, page_no_ * PageSize) != PageSize)
		return Fail;
	return Success;
}

BTreePage* BTreePageBucket::GetPage(const page_id page_no, const int fd)
{
	int index = Max;
	uint16_t fresh = 0xFFFF;
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
	assert(index != Max);
	if (len_ == Max) {
		assert(pages_[index]->Write(fd));
		ages_[index] = 0;
		assert(pages_[index]->Read(fd));
		return pages_[index];
	} else {
		BTreePage *page = BTreePage::NewPage(page_no);
		if (page) {
			pages_[index] = page;
			++len_;
		}
		return page;
	}
}

Status BTreePageBucket::PinPage(BTreePage *page, const int fd)
{
	int index = Max;
	uint16_t fresh = 0xFFFF;
	if (len_ < Max) {
		pages_[len_++] = page;
		return Success;
	}
	for (int i = 0; i != Max; ++i) {
		if (ages_[i] < fresh && !pages_[i]->Occupy()) {
			index = i;
			fresh = ages_[i];
		}
	}
	assert(index != Max);
	assert(pages_[index]->Write(fd));
	ages_[index]  = 0;
	pages_[index] = page;
	return Success;
}

BTreePage* BTreePager::GetPage(const page_id page_no)
{
	BTreePageBucket &bucket = bucket_[page_no & Mask];
	BTreePage *page;
	if (page_no < total_) {
		page = bucket.GetPage(page_no, fd_);
		if (page) ++total_;
		return page;
	} else {
		page = BTreePage::NewPage(page_no);
		if (page) {
			assert(bucket.PinPage(page, fd_));
			++total_;
		}
	}
	return page;
}

Status BTreePageBucket::Close(const int fd)
{
	for (int i = len_ - 1; i >= 0; --i) {
		assert(pages_[i]->Write(fd));
		free(pages_[i]);
		ages_[i] = 0;
		--len_;
	}
	return Success;
}

Status BTreePager::Close()
{
	for (int i = 0; i != Hash; ++i)
		assert(bucket_[i].Close(fd_));
	if (fd_ > 0)
		close(fd_);
	fd_ = -1;
	return Success;
}

} // namespace Mushroom
