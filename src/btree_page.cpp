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
	snprintf(no, 16, "%d  ", level_);
	str += "level: " + std::string(no);
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

bool BTreePage::Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, bool ge) const
{
	int low = 0, high = total_key_;
	uint16_t *index = Index();
	KeySlice *curr = nullptr;
	while (low != high) {
		int mid = low + ((high - low) >> 1);
		curr = (KeySlice *)(data_ + index[mid]);
		int res = Compare(key, curr, key_len_);
		if (res < 0) {
			high = mid;
		} else if (res > 0) {
			low = mid + 1;
		} else {
			*idx = mid;
			*slice = curr;
			if (!ge) {
				return true;
			} else {
				low = ++mid;
				curr = (KeySlice *)(data_ + index[mid]);
			}
		}
	}
	*idx = high;
	*slice = curr;
	return false;
}

page_id BTreePage::Descend(const KeySlice *key) const
{
	assert(type_ != LEAF);
	uint16_t index;
	KeySlice *slice = nullptr;
	Traverse(key, &index, &slice, true);
	assert(slice);
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
	return true;
}

bool BTreePage::Search(KeySlice *key, page_id *page_no) const
{
	assert(type_ != BRANCH);
	uint16_t index;
	KeySlice *slice = nullptr;
	if (Traverse(key, &index, &slice)) {
		*page_no = page_no_;
		key->AssignPageNo(slice->PageNo());
		return true;
	}
	return false;
}

bool BTreePage::FindGreatEq(KeySlice *key, page_id *page_no) const
{
	assert(type_ != BRANCH);
	uint16_t index;
	KeySlice *slice;
	Traverse(key, &index, &slice, true);
	if (index < (total_key_-1)) {
		*page_no = page_no_;
		memcpy(key, slice, DataId + key_len_);
		return true;
	} else if (index == (total_key_-1)) {
		*page_no = page_no_;
		return false;
	} else {
		*page_no = 0;
		return false;
	}
}

void BTreePage::Split(BTreePage *that, KeySlice *slice)
{
	uint16_t left = total_key_ >> 1, right = total_key_ - left;
	uint16_t *l_idx = this->Index();
	uint16_t *r_idx = that->Index() - right;
	std::cout << ((char *)r_idx - (char *)that) << std::endl;
	uint16_t slot_len = DataId + key_len_;
	KeySlice *fence = (KeySlice *)(this->data_ + l_idx[left]);

	if (type_ == BRANCH)
		that->AssignFirst(fence->PageNo());
	fence->AssignPageNo(that->PageNo());

	slice->Assign(that->PageNo(), fence->Data(), key_len_);

	for (uint16_t i = left++, j = 0; i != total_key_; ++i, ++j) {
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
	uint16_t limit = left * slot_len, offset = 0;
	for (uint16_t i = left; i != total_key_; ++i) {
		if (l_idx[i] < limit && offset < left) {
			for (uint16_t j = offset; j < left; offset = ++j) {
				if (l_idx[j] >= limit) {
					KeySlice *o = (KeySlice *)(that->data_ + l_idx[i]);
					KeySlice *n = (KeySlice *)(that->data_ + l_idx[j]);
					l_idx[j] = l_idx[i];
					memcpy(o, n, slot_len);
					++offset;
					break;
				}
			}
		}
		if (offset == left) break;
	}
	this->total_key_ = left;
	that->total_key_ = right;
	// this->Info();
	// that->Info();
}

void BTreePage::Info() const
{
	std::string str = ToString();
	if (!total_key_) {
		std::cout << str;
		return ;
	}

	uint16_t *index = Index();
	char no[8];
	for (uint16_t i = 0; i != total_key_; ++i) {
		snprintf(no, 8, "%d ", index[i]);
		str += std::string(no);
	}
	std::cout << str << std::endl;

	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)(data_ + index[i]);
		std::cout << key;
	}
	std::cout << std::endl;
}

BTreePage* BTreePage::NewPage(page_id page_no, int type, uint8_t key_len, uint8_t level)
{
	BTreePage *page = (BTreePage *)new char[BTreePage::PageSize];
	if (!page) return page;
	page->page_no_ = page_no;
	page->type_    = type;
	page->key_len_ = key_len;
	page->level_   = level;
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
	int index = len_;
	uint16_t fresh = 0xFFFF;
	for (int i = 0; i != len_; ++i) {
		if (pages_[i] && page_no == pages_[i]->PageNo()) {
			++ages_[i];
			return pages_[i];
		}
		if (!pages_[i] || (ages_[i] < fresh && !pages_[i]->Occupy())) {
			index = i;
			fresh = ages_[i];
		}
	}
	assert(index != len_);
	if (len_ != Max) {
		BTreePage *page = BTreePage::NewPage(page_no, 0, 0, 0);
		if (page) {
			assert(page->Read(fd));
			pages_[index] = page;
			++len_;
			return page;
		}
	}
	assert(pages_[index]->Write(fd));
	ages_[index] = 0;
	assert(pages_[index]->Read(fd));
	return pages_[index];
}

Status BTreePageBucket::PinPage(BTreePage *page, const int fd)
{
	if (len_ < Max) {
		pages_[len_++] = page;
		return Success;
	}
	int index = Max;
	uint16_t fresh = 0xFFFF;
	for (int i = 0; i != len_; ++i) {
		if (!pages_[i] || (ages_[i] < fresh && !pages_[i]->Occupy())) {
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

Status BTreePageBucket::UnPinPage(BTreePage *page, const int fd)
{
	for (int i = 0; i != len_; ++i)
		if (pages_[i] == page) {
			assert(pages_[i]->Write(fd));
			ages_[i] = 0;
			pages_[i] = nullptr;
			return Success;
		}
	assert(0);
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
	BTreePage *page = BTreePage::NewPage(curr_, type, key_len, level);
	if (page) {
		BTreePageBucket &bucket = bucket_[curr_ & Mask];
		assert(bucket.PinPage(page, fd_));
		++curr_;
	}
	return page;
}

Status BTreePager::PinPage(BTreePage *page)
{
	assert(page->PageNo() < curr_);
	BTreePageBucket &bucket = bucket_[page->PageNo() & Mask];
	bucket.PinPage(page, fd_);
	return Success;
}

Status BTreePager::UnPinPage(BTreePage *page)
{
	assert(page->PageNo() < curr_);
	BTreePageBucket &bucket = bucket_[page->PageNo() & Mask];
	bucket.UnPinPage(page, fd_);
	return Success;
}

Status BTreePageBucket::Clear(const int fd)
{
	for (int i = len_ - 1; i >= 0; --i) {
		if (pages_[i]) {
			assert(pages_[i]->Write(fd));
			delete [] pages_[i];
		}
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
