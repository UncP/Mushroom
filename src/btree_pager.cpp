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
#include <iostream>

#include "btree_pager.hpp"
#include "utility.hpp"

namespace Mushroom {

std::string BTreePage::ToString() const
{
	std::string str("type: ");
	if (type_ == LEAF)   str += "leaf  ";
	if (type_ == BRANCH) str += "branch  ";
	if (type_ == ROOT)   str += "root  ";
	char no[16];
	snprintf(no, 16, "%d  ", page_no_);
	str += "page_no: " + std::string(no);
	snprintf(no, 16, "%d  ", first_);
	str += "first: " + std::string(no);
	snprintf(no, 16, "%d  ", total_key_);
	str += "tot_key: " + std::string(no);
	snprintf(no, 16, "%d  ", level_);
	str += "level: " + std::string(no);
	if (dirty_)
		str += "dirty: true\n";
	else
		str += "dirty: false\n";

	uint16_t *index = Index();
	assert(total_key_ < 500);
	for (uint16_t i = 0; i != total_key_; ++i) {
		snprintf(no, 16, "%d ", index[i]);
		str += std::string(no);
	}
	str += '\n';
	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)(data_ + index[i]);
		str += key->ToString();
	}
	str += '\n';
	return std::move(str);
}

std::string BTreePageBucket::ToString() const
{
	if (!len_) return std::string("\n");
	std::string str("len: ");
	char no[16];
	snprintf(no, 16, "%d\n", len_);
	str += std::string(no);
	for (int i = 0; i != len_; ++i) {
		snprintf(no, 16, "%d ", ages_[i]);
		str += std::string(no);
	}
	str += '\n';
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

bool BTreePage::Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type) const
{
	int low = 0, high = total_key_, mid = 0;
	uint16_t *index = Index();
	KeySlice *curr = nullptr;
	while (low != high) {
		mid = low + ((high - low) >> 1);
		curr = Key(index, mid);
		int res = Compare(key, curr, key_len_);
		if (res < 0) {
			high = mid;
		} else if (res > 0) {
			low = mid + 1;
		} else {
			*idx = mid;
			*slice = curr;
			if (type == Eq) {
				return true;
			} else {
				low = mid + 1;
				curr = Key(index, low);
			}
		}
	}
	*idx = high;
	if (type == Ge && high)
		*slice = Key(index, high-1);
	return false;
}

page_id BTreePage::Descend(const KeySlice *key) const
{
	assert(type_ != LEAF);
	uint16_t index;
	KeySlice *slice = nullptr;
	Traverse(key, &index, &slice, Ge);
	return index ? slice->PageNo() : first_;
}

bool BTreePage::Insert(const KeySlice *key)
{
	uint16_t pos;
	KeySlice *slice = nullptr;
	if (Traverse(key, &pos, &slice)) return false;

	uint16_t slot_len = DataId + key_len_, end = total_key_ * slot_len;
	memcpy(data_ + end, key, slot_len);

	uint16_t *index = Index();
	--index;
	if (pos) memmove(&index[0], &index[1], pos << 1);
	index[pos] = end;
	++total_key_;
	dirty_ = true;
	return true;
}

bool BTreePage::Search(KeySlice *key) const
{
	assert(type_ != BRANCH);
	uint16_t index;
	KeySlice *slice = nullptr;
	return Traverse(key, &index, &slice);
}

bool BTreePage::Ascend(KeySlice *key, page_id *page_no, uint16_t *idx)
{
	uint16_t *index = Index();
	if (*idx < (total_key_ - 1)) {
		memcpy(key, Key(index, *idx), DataId + key_len_);
		++*idx;
		return true;
	} else {
		*page_no = Key(index, *idx)->PageNo();
		*idx = 0;
		return false;
	}
}

void BTreePage::Split(BTreePage *that, KeySlice *slice)
{
	uint16_t left = total_key_ >> 1, right = total_key_ - left, index = left;
	uint16_t *l_idx = this->Index();
	uint16_t *r_idx = that->Index();
	uint16_t slot_len = DataId + key_len_;
	KeySlice *fence = (KeySlice *)(this->data_ + l_idx[left++]);

	slice->Assign(that->PageNo(), fence->Data(), key_len_);

	if (level_) {
		that->AssignFirst(fence->PageNo());
		memcpy(fence->Data(), Key(l_idx, left)->Data(), key_len_);
		r_idx -= --right;
		++index;
	} else {
		r_idx -= right;
	}

	fence->AssignPageNo(that->PageNo());

	for (uint16_t i = index, j = 0; i != total_key_; ++i, ++j) {
		r_idx[j] = j * slot_len;
		KeySlice *l = this->Key(l_idx, i);
		KeySlice *r = that->Key(r_idx, j);
		memcpy(r, l, slot_len);
	}
	uint16_t limit = left * slot_len, j = 0;
	for (uint16_t i = left; i < total_key_ && j < left; ++i) {
		if (l_idx[i] < limit) {
			for (; j < left; ++j) {
				if (l_idx[j] >= limit) {
					KeySlice *o = this->Key(l_idx, i);
					KeySlice *n = this->Key(l_idx, j);
					l_idx[j] = l_idx[i];
					memcpy(o, n, slot_len);
					++j;
					break;
				}
			}
		}
	}

	uint16_t offset = total_key_ - left;
	memmove(&l_idx[offset], &l_idx[0], left << 1);

	this->total_key_ = left;
	that->total_key_ = right;
	this->dirty_ = true;
	that->dirty_ = true;
}

BTreePage* BTreePage::NewPage(page_id page_no, int type, uint8_t key_len, uint8_t level)
{
	BTreePage *page = (BTreePage *)new char[BTreePage::PageSize];
	if (!page) return page;
	page->Reset(page_no, type, key_len, level);
	return page;
}

Status BTreePage::Write(const int fd)
{
	if (dirty_) {
		dirty_ = 0;
		if (pwrite(fd, this, (size_t)PageSize, page_no_ * PageSize) != PageSize)
			return Fail;
	}
	return Success;
}

Status BTreePage::Read(const page_id page_no, const int fd)
{
	if (pread(fd, this, PageSize, page_no * PageSize) != PageSize)
		return Fail;
	assert(page_no_ == page_no);
	return Success;
}

BTreePage* BTreePageBucket::GetPage(const page_id page_no, const int fd)
{
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

BTreePage* BTreePager::NewPage(int type, uint8_t key_len, uint8_t level)
{
	BTreePageBucket &bucket = bucket_[curr_ & Mask];
	BTreePage *page = BTreePage::NewPage(curr_, type, key_len, level);
	if (page) {
		assert(bucket.PinPage(page, fd_));
	} else {
		page = bucket.GetEmptyPage(curr_, type, key_len, level, fd_);
	}
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
		if (pages_[i]) {
			// assert(pages_[i]->Write(fd));
			delete [] pages_[i];
		}
		ages_[i] = 0;
		--len_;
	}
	return Success;
}

Status BTreePager::Close()
{
	getchar();
	for (int i = 0; i != Hash; ++i)
		assert(bucket_[i].Clear(fd_));
	if (fd_ > 0)
		close(fd_);
	fd_ = -1;
	return Success;
}

} // namespace Mushroom
