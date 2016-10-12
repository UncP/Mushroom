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

#include "btree_page.hpp"

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
	int low = 0, high = total_key_;
	uint16_t *index = (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
	KeySlice *curr = nullptr;
	while (low != high) {
		int mid = low + ((high - low) >> 1);
		curr = (KeySlice *)(data_ + index[mid]);
		int res = compare(&key, curr, key.Length());
		if (res < 0) {
			high = mid;
		} else if (res > 0) {
			low = mid + 1;
		} else {
			curr = (KeySlice *)(data_ + index[mid+1]);
			break;
		}
	}
	assert(curr);
	return high ? curr->PageNo() : first_;
}

bool BTreePage::Insert(const Slice &key)
{
	int low = 0, high = total_key_, pos = low + ((high - low) >> 1);
	uint16_t *index = (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
	while (low != high) {
		KeySlice *curr = (KeySlice *)(data_ + index[pos]);
		int res = compare(&key, curr, key_len_);
		if (res < 0)
			high = pos;
		else if (res > 0)
			low = pos + 1;
		else
			return false;
		pos = low + ((high - low) >> 1);
	}
	uint16_t slot_len = DataId + key_len_, end = total_key_ * slot_len;
	memcpy(data_ + end + DataId, key.Data(), key_len_);
	--index;
	if (pos) memmove(&index[0], &index[1], pos << 1);
	index[pos] = end;
	++total_key_;
	return true;
}

bool BTreePage::Insert(const KeySlice *key)
{
	int low = 0, high = total_key_, pos = low + ((high - low) >> 1);
	uint16_t *index = (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
	while (low != high) {
		KeySlice *curr = (KeySlice *)(data_ + index[pos]);
		int res = compare(key, curr, key_len_);
		if (res < 0)
			high = pos;
		else if (res > 0)
			low = pos + 1;
		else
			return false;
		pos = low + ((high - low) >> 1);
	}
	uint16_t slot_len = DataId + key_len_, end = total_key_ * slot_len;
	memcpy(data_ + end, key, slot_len);
	--index;
	if (pos) memmove(&index[0], &index[1], pos << 1);
	index[pos] = end;
	++total_key_;
	return true;
}

void BTreePage::Split(BTreePage *that, char *key)
{
	uint16_t left = total_key_ >> 1, right = total_key_ - left;
	uint16_t *l_idx = (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
	uint16_t *r_idx = (uint16_t *)((char *)that + (PageSize - (right * IndexByte)));
	uint16_t slot_len = DataId + key_len_;
	if (type_ == BRANCH) {
		KeySlice *l = (KeySlice *)(this->data_ + l_idx[left++]);
		that->AssignFirst(l->PageNo());
		memcpy(key, l->Data(), key_len_);
	}
	for (uint16_t i = left, j = 0; i != total_key_; ++i, ++j) {
		r_idx[j] = j * slot_len;
		KeySlice *l = (KeySlice *)(this->data_ + l_idx[i]);
		KeySlice *r = (KeySlice *)(that->data_ + r_idx[j]);
		memcpy(r, l, slot_len);
		// if (l_idx[i] < limit && offset < left) {
		// 	for (uint16_t k = offset; k < left; offset = ++k) {
		// 		if (l_idx[k] >= limit) {
		// 			KeySlice *tmp = (KeySlice *)(data_ + l_idx[k]);
		// 			l_idx[k] = l_idx[i];
		// 			memcpy(l, tmp, slot_len);
		// 			++offset;
		// 			break;
		// 		}
		// 	}
		// }
	}
	Compact();
	that->total_key_ = left;
	that->total_key_ = right;
	that->right_ = this->right_;
	this->right_ = that->page_no_;
}

void BTreePage::Compact()
{
	uint16_t left = total_key_ >> 1;
	uint16_t *index = (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
	uint16_t slot_len = DataId + key_len_;
	uint16_t limit = left * slot_len, offset = 0;
	for (uint16_t i = left; i != total_key_; ++i) {
		if (index[i] < limit && offset < left) {
			for (uint16_t j = offset; j < left; offset = ++j) {
				if (index[j] >= limit) {
					KeySlice *o = (KeySlice *)(data_ + index[i]);
					KeySlice *n = (KeySlice *)(data_ + index[j]);
					index[j] = index[i];
					memcpy(o, n, slot_len);
					++offset;
					break;
				}
			}
		}
		if (offset == left) break;
	}
}

void BTreePage::Info(const uint8_t key_len) const
{
	std::string str = ToString();
	if (!total_key_) {
		std::cout << str;
		return ;
	}

	uint16_t *index = (uint16_t *)((char *)this + (PageSize - (total_key_ * IndexByte)));
	char no[8];
	for (uint16_t i = 0; i != total_key_; ++i) {
		snprintf(no, 8, "%d ", index[i]);
		str += std::string(no);
	}
	str += "\n";

	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)(data_ + index[i]);
		assert(!key->DataEmpty());
		str += std::string(key->Data(), key_len) + " ";
	}
	std::cout << str << std::endl;
}

BTreePage* BTreePage::NewPage(const page_id page_no, int type, uint8_t key_len)
{
	BTreePage *page = (BTreePage *)calloc(1, BTreePage::PageSize);
	assert(page);
	page->page_no_ = page_no;
	page->type_ = type;
	page->key_len_ = key_len;
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
		BTreePage *page = BTreePage::NewPage(page_no, 0, 0);
		if (page) {
			assert(page->Read(fd));
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
	assert(page_no < curr_);
	BTreePageBucket &bucket = bucket_[page_no & Mask];
	BTreePage *page = bucket.GetPage(page_no, fd_);
	return page;
}

BTreePage* BTreePager::NewPage(int type, uint8_t key_len)
{
	BTreePage *page = BTreePage::NewPage(curr_, type, key_len);
	if (page) {
		BTreePageBucket &bucket = bucket_[curr_ & Mask];
		assert(bucket.PinPage(page, fd_));
		++curr_;
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
