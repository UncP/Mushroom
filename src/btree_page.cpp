/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 09:10:01
**/

#include <unistd.h>
#include <sstream>
#include <set>

#include "btree_page.hpp"
#include "utility.hpp"

namespace Mushroom {

uint16_t BTreePage::CalculateDegree(uint8_t key_len, uint8_t pre_len)
{
	BTreePage *page = nullptr;
	uint16_t offset = (char *)page->data_ - (char *)page + pre_len;
	return (PageSize - offset) / (PageByte + IndexByte + key_len);
}

void BTreePage::Reset(page_id page_no, int type, uint8_t key_len, uint8_t level,
	uint16_t degree)
{
	assert(!dirty_);
	memset(this, 0, PageSize);
	page_no_ = page_no;
	degree_  = degree;
	type_    = type;
	key_len_ = key_len;
	level_   = level;
}

bool BTreePage::Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type) const
{
	int low = 0, high = total_key_, mid = 0;
	uint16_t *index = Index();
	KeySlice *curr = nullptr;
	if (pre_len_) {
		int res = ComparePrefix(key, data_, pre_len_);
		if (res < 0) {
			*idx = 0;
			return false;
		} else if (res > 0) {
			assert(0);
		}
	}
	while (low != high) {
		mid = low + ((high - low) >> 1);
		curr = Key(index, mid);
		int res = CompareSuffix(key, curr, pre_len_, key_len_);
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
	if (high) *slice = Key(index, high - 1);
	return false;
}

page_id BTreePage::Descend(const KeySlice *key) const
{
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

	uint16_t slot_len = PageByte + key_len_, end = total_key_ * slot_len;
	memcpy(data_ + end, key, slot_len);

	uint16_t *index = Index();
	--index;
	if (pos) memmove(&index[0], &index[1], pos << 1);
	index[pos] = end;
	++total_key_;
	dirty_ = true;
	return true;
}

bool BTreePage::Insert(const KeySlice *key, page_id &page_no)
{
	uint16_t *index = Index();
	KeySlice *fence = Key(index, total_key_ - 1);
	if (pre_len_) {
		int res = ComparePrefix(key, data_, pre_len_);
		if (res < 0) {
			// TODO
		} else if (res > 0) {
			assert(0);
		}
	}
	int res = CompareSuffix(key, fence, pre_len_, key_len_);
	if (res < 0) {
		assert(Insert(key));
		return true;
	} else if (res > 0) {
		page_no = fence->PageNo();
		assert(page_no);
	}
	return false;
}

bool BTreePage::Search(KeySlice *key) const
{
	uint16_t index;
	KeySlice *slice = nullptr;
	return Traverse(key, &index, &slice);
}

// bool BTreePage::Ascend(KeySlice *key, page_id *page_no, uint16_t *idx)
// {
// 	uint16_t *index = Index();
// 	if (*idx < (total_key_ - 1)) {
// 		CopyKey(key, Key(index, *idx), PageByte + key_len_);
// 		++*idx;
// 		return true;
// 	} else {
// 		*page_no = Key(index, *idx)->PageNo();
// 		*idx = 0;
// 		return false;
// 	}
// }

void BTreePage::Split(BTreePage *that, KeySlice *slice)
{
	uint16_t left = total_key_ >> 1, right = total_key_ - left, index = left;
	uint16_t *l_idx = this->Index();
	uint16_t *r_idx = that->Index();
	uint16_t slot_len = PageByte + key_len_;
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
		CopyKey(r, l, slot_len);
	}
	uint16_t limit = left * slot_len, j = 0;
	for (uint16_t i = left; i < total_key_ && j < left; ++i) {
		if (l_idx[i] < limit) {
			for (; j < left; ++j) {
				if (l_idx[j] >= limit) {
					KeySlice *o = this->Key(l_idx, i);
					KeySlice *n = this->Key(l_idx, j);
					l_idx[j] = l_idx[i];
					CopyKey(o, n, slot_len);
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

BTreePage* BTreePage::NewPage(page_id page_no, int type, uint8_t key_len, uint8_t level,
	uint16_t degree)
{
	BTreePage *page = (BTreePage *)new char[PageSize];
	if (!page) return page;
	page->Reset(page_no, type, key_len, level, degree);
	return page;
}

Status BTreePage::Read(const page_id page_no, const int fd)
{
	if (pread(fd, this, PageSize, page_no * PageSize) != PageSize)
		return Fail;
	assert(page_no_ == page_no);
	return Success;
}

Status BTreePage::Write(const int fd)
{
	if (dirty_) {
		dirty_ = 0;
		if (pwrite(fd, this, PageSize, page_no_ * PageSize) != PageSize)
			return Fail;
	}
	return Success;
}

std::string BTreePage::ToString() const
{
	std::ostringstream os;
	os << "type: ";
	if (type_ == LEAF)   os << "leaf  ";
	if (type_ == BRANCH) os << "branch  ";
	if (type_ == ROOT)   os << "root  ";
	os << "page_no: " << page_no_ << " ";
	os << "first: " << first_ << " ";
	os << "tot_key: " << total_key_ << " ";
	os << "level: " << (int)level_ << " ";
	if (dirty_)
		os << "dirty: true\n";
	else
		os << "dirty: false\n";

	uint16_t *index = Index();
	for (uint16_t i = 0; i != total_key_; ++i)
		os << index[i] << " ";
	os << "\n";
	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)(data_ + index[i]);
		os << key->ToString();
	}
	os << "\n";
	return os.str();
}

void BTreePage::Analyze() const
{
	std::set<uint16_t> offsets;
	std::set<std::string> keys;
	uint16_t *index = Index();
	for (uint16_t i = 0; i != total_key_; ++i)
		if (offsets.find(index[i]) == offsets.end())
			offsets.insert(index[i]);
		else
			std::cout << "repeated index " << index[i] << std::endl;
	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)(data_ + index[i]);
		std::string tmp(key->Data(), key_len_);
		if (keys.find(tmp) == keys.end())
			keys.insert(tmp);
		else
			std::cout << "repeated key " << tmp << std::endl;
	}
}

void BTreePage::Compact()
{

}

} // namespace Mushroom
