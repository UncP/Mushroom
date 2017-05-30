/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-22 09:10:01
**/

#include <cassert>

#include "page.hpp"

namespace Mushroom {

uint32_t Page::PageSize;

void Page::SetPageInfo(uint32_t page_size)
{
	PageSize = page_size;
}

uint16_t Page::CalculateDegree(uint8_t key_len, uint8_t pre_len)
{
	Page *page = 0;
	uint16_t offset = (char *)page->data_ - (char *)page + pre_len;
	return (PageSize - offset) / (PageByte + IndexByte + key_len);
}

Page* Page::NewPage(uint8_t key_len)
{
	char *buf = new char[PageSize];
	memset(buf, 0, PageSize);
	Page *page = (Page *)buf;
	page->key_len_ = key_len;
	page->degree_  = CalculateDegree(key_len);
	page->InsertInfiniteKey();
	return page;
}

void Page::DeletePage(Page *page)
{
	delete [] (char *)page;
}

Page::Page(page_t page_no, uint8_t type, uint8_t key_len, uint8_t level, uint16_t degree)
{
	memset(this, 0, PageSize);
	page_no_ = page_no;
	degree_  = degree;
	type_    = (uint8_t)type;
	key_len_ = key_len;
	level_   = level;
}

void Page::InsertInfiniteKey()
{
	TempSlice(key, key_len_);
	memset(key->key_, 0xFF, key_len_);
	page_t page_no = 0;
	assert(Insert(key, page_no) == InsertOk);
}

void Page::AssignFirst(page_t first)
{
	first_ = first;
}

bool Page::Traverse(const KeySlice *key, uint16_t *idx, KeySlice **slice, int type) const
{
	uint16_t low = 0, high = total_key_, mid = 0;
	uint16_t *index = Index();
	if (pre_len_) {
		int res = ComparePrefix(key, data_, pre_len_);
		if (res < 0) {
			*idx = 0;
			return false;
		} else if (res > 0) {
			*idx = high--;
			*slice = Key(index, high);
			return false;
		}
	}
	KeySlice *curr = 0;
	while (low != high) {
		mid = low + ((high - low) >> 1);
		curr = Key(index, mid);
		int res = CompareSuffix(key, curr, pre_len_, key_len_);
		if (res < 0) {
			high = mid;
		} else if (res > 0) {
			low = mid + 1;
		} else {
			if (type) {
				*idx = mid;
				return true;
			} else {
				low = mid + 1;
			}
		}
	}
	*idx = high;
	if (high) *slice = Key(index, high-1);
	return false;
}

page_t Page::Descend(const KeySlice *key) const
{
	uint16_t index;
	KeySlice *slice = 0;
	Traverse(key, &index, &slice, 0);
	return index ? slice->page_no_ : first_;
}

bool Page::Search(KeySlice *key, uint16_t *index) const
{
	KeySlice *slice = 0;
	return Traverse(key, index, &slice);
}

InsertStatus Page::Insert(const KeySlice *key, page_t &page_no)
{
	uint16_t pos;
	KeySlice *slice = 0;
	bool flag = Traverse(key, &pos, &slice);
	if (flag)
		return ExistedKey;
	if (pos == total_key_ && pos) {
		page_no = Next();
		assert(page_no);
		return MoveRight;
	}

	uint16_t end = total_key_ * (PageByte + key_len_) + pre_len_;
	memcpy(data_ + end, &key->page_no_, PageByte);
	memcpy(data_ + end + PageByte, key->key_ + pre_len_, key_len_);

	uint16_t *index = Index();
	--index;
	if (pos) memmove(&index[0], &index[1], pos << 1);
	index[pos] = end;
	++total_key_;
	dirty_ = 1;
	return InsertOk;
}

bool Page::Insert(const KeySlice *key)
{
	page_t page_no;
	return Insert(key, page_no) == InsertOk;
}

void Page::Split(Page *that, KeySlice *slice)
{
	uint16_t left = total_key_ >> 1, right = total_key_ - left, index = left;
	uint16_t *l_idx = this->Index();
	uint16_t *r_idx = that->Index();
	KeySlice *fence = Key(l_idx, left++);

	if (pre_len_) {
		memcpy(that->data_, this->data_, pre_len_);
		that->pre_len_ = this->pre_len_;
		CopyPrefix(slice, data_, pre_len_);
	}

	slice->page_no_ = that->page_no_;
	memcpy(slice->key_ + pre_len_, fence->key_, key_len_);

	if (level_) {
		that->AssignFirst(fence->page_no_);
		memcpy(fence->key_, Key(l_idx, left)->key_, key_len_);
		r_idx -= --right;
		++index;
	} else {
		r_idx -= right;
	}

	fence->page_no_ = that->page_no_;

	uint16_t slot_len = PageByte + key_len_;
	for (uint16_t i = index, j = 0; i != total_key_; ++i, ++j) {
		r_idx[j] = that->pre_len_ + j * slot_len;
		KeySlice *l = this->Key(l_idx, i);
		KeySlice *r = that->Key(r_idx, j);
		CopyKey(r, l, 0, key_len_);
	}
	uint16_t limit = left * slot_len + pre_len_, j = 0;
	for (uint16_t i = left; i < total_key_ && j < left; ++i) {
		if (l_idx[i] < limit) {
			for (; j < left; ++j) {
				if (l_idx[j] >= limit) {
					KeySlice *o = this->Key(l_idx, i);
					KeySlice *n = this->Key(l_idx, j);
					l_idx[j] = l_idx[i];
					CopyKey(o, n, 0, key_len_);
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
	this->dirty_ = 1;
	that->dirty_ = 1;
}

bool Page::Full() const
{
	return total_key_ == degree_;
}

bool Page::NeedSplit()
{
	if (!Full())
		return false;
	uint16_t *index = Index();
	const char *first = Key(index, 0)->key_;
	const char *last  = Key(index, total_key_ - 1)->key_;
	char prefix[key_len_];
	uint8_t pre_len = 0;
	for (; first[pre_len] == last[pre_len]; ++pre_len)
		prefix[pre_len] = first[pre_len];
	if (!pre_len)
		return true;
	uint16_t degree = CalculateDegree(key_len_ - pre_len, pre_len + pre_len_);
	if (degree <= degree_)
		return true;
	char buf[PageSize];
	Page *copy = (Page *)buf;
	memcpy(copy, this, PageSize);
	memcpy(data_ + pre_len_, prefix, pre_len);
	char *curr = data_ + pre_len_ + pre_len;
	uint16_t *cindex = copy->Index();
	uint16_t suf_len = key_len_ - pre_len;
	for (uint16_t i = 0; i != total_key_; ++i, ++index) {
		KeySlice *key = copy->Key(cindex, i);
		*index = curr - data_;
		memcpy(curr, &key->page_no_, PageByte);
		curr += PageByte;
		memcpy(curr, key->key_ + pre_len, suf_len);
		curr += suf_len;
	}
	pre_len_ += pre_len;
	key_len_ -= pre_len;
	degree_  = degree;
	dirty_   = 1;
	return false;
}

} // namespace Mushroom
