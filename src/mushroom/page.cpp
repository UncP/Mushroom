/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-22 09:10:01
**/

#include <cassert>
#include <sstream>

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
	page->type_    = LEAF;
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
				*slice = Key(index, *idx);
				return true;
			} else {
				low = mid + 1;
			}
		}
	}
	*idx = high;
	if (high) *slice = Key(index, high - 1);
	return false;
}

page_t Page::Descend(const KeySlice *key, bool &is_last) const
{
	uint16_t index;
	KeySlice *slice = 0;
	Traverse(key, &index, &slice, 0);
	is_last = (index == (total_key_ - 1));
	return index ? slice->page_no_ : first_;
}

bool Page::Search(const KeySlice *key, uint16_t *index) const
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

UpdateStatus Page::Update(const KeySlice *old_key, const KeySlice *new_key)
{
	uint16_t pos;
	KeySlice *slice = 0;
	bool flag = Traverse(old_key, &pos, &slice);
	if (flag) {
		memcpy(slice->key_, new_key->key_ + pre_len_, key_len_);
		if (pos != total_key_ - 1)
			return UpdateOk;
		else
			return Promote;
	} else {
		assert(pos != total_key_); // should not move to next page on the same level
		return Promote;
	}
}

void Page::FillFrom(uint16_t above)
{
	uint16_t *index = Index();
	uint16_t limit = above * (PageByte + key_len_) + pre_len_, j = 0;
	for (uint16_t i = above; i < total_key_ && j < above; ++i) {
		if (index[i] < limit) {
			for (; j < above; ++j) {
				if (index[j] >= limit) {
					KeySlice *o = Key(index, i);
					KeySlice *n = Key(index, j);
					index[j] = index[i];
					CopyKey(o, n, 0, key_len_);
					++j;
					break;
				}
			}
		}
	}
	uint16_t offset = total_key_ - above;
	memmove(&index[offset], &index[0], above << 1);
	total_key_ = above;
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
	CopySuffix(slice, fence->key_, pre_len_, key_len_);

	if (level_) {
		that->AssignFirst(fence->page_no_);
		r_idx -= --right;
		++index;
		// KeySlice *new_fence = Key(l_idx, left);
		// CopySuffix(fence, new_fence->key_, pre_len_, key_len_);
	} else {
		r_idx -= right;
	}

	uint16_t slot_len = PageByte + key_len_;
	for (uint16_t i = index, j = 0; i != total_key_; ++i, ++j) {
		r_idx[j] = that->pre_len_ + j * slot_len;
		KeySlice *l = this->Key(l_idx, i);
		KeySlice *r = that->Key(r_idx, j);
		CopyKey(r, l, 0, key_len_);
	}

	fence->page_no_ = that->page_no_;

	FillFrom(left);

	that->total_key_ = right;
	this->dirty_ = 1;
	that->dirty_ = 1;
}

void Page::Expand(uint8_t to)
{
	uint8_t pre = pre_len_ - to;
	char prefix[pre];
	memcpy(prefix, data_ + to, pre);
	char tmp[PageSize];
	Page *copy = (Page *)tmp;
	memcpy(copy, this, PageSize);
	uint16_t *index = Index();
	char *curr = data_ + to;
	uint16_t *cindex = copy->Index();
	for (uint16_t i = 0; i < total_key_; ++i) {
		KeySlice *key = copy->Key(cindex, i);
		index[i] = curr - data_;
		memcpy(curr, &key->page_no_, PageByte);
		curr += PageByte;
		memcpy(curr, prefix, pre);
		curr += pre;
		memcpy(curr, key->key_, key_len_);
		curr += key_len_;
	}
	pre_len_ = to;
	key_len_ += pre;
	degree_ = CalculateDegree(key_len_, pre_len_);
}

bool Page::Move(Page *that, KeySlice *old_key, KeySlice *new_key)
{
	assert(this->FenceKeyEqual(that));
	uint16_t max_key = this->pre_len_ < that->pre_len_ ? this->degree_ : that->degree_;
	// make sure at least move 2 keys
	if (that->total_key_ + 4 > max_key)
		return false;

	uint16_t move = (max_key - that->total_key_) >> 1;

	uint16_t index = total_key_ - 1 - move;
	uint16_t *l_idx = this->Index();
	uint16_t *r_idx = that->Index();

	KeySlice *fence = Key(l_idx, total_key_ - 1);
	if (this->pre_len_) CopyPrefix(old_key, data_, this->pre_len_);
	CopyKey(old_key, fence, this->pre_len_, key_len_);
	old_key->page_no_ = that->page_no_;

	fence = Key(l_idx, index);
	if (this->pre_len_) CopyPrefix(new_key, data_, this->pre_len_);
	CopyKey(new_key, fence, this->pre_len_, key_len_);
	new_key->page_no_ = that->page_no_;

	r_idx -= move;
	if (this->pre_len_ == that->pre_len_) {
		uint16_t slot_len = PageByte + that->key_len_;
		uint16_t limit = that->total_key_ * slot_len + that->pre_len_;
		for (uint16_t i = index, j = 0, end = index + move; i != end; ++i, ++j) {
			r_idx[j] = limit + j * slot_len;
			KeySlice *l = this->Key(l_idx, i);
			KeySlice *r = that->Key(r_idx, j);
			CopyKey(r, l, 0, key_len_);
		}
	} else if (this->pre_len_ > that->pre_len_) {
		uint16_t slot_len = PageByte + that->key_len_;
		uint16_t limit = that->total_key_ * slot_len + that->pre_len_;
		uint16_t left = this->pre_len_ - that->pre_len_;
		for (uint16_t i = index, j = 0, end = index + move; i != end; ++i, ++j) {
			r_idx[j] = limit + j * slot_len;
			KeySlice *l = this->Key(l_idx, i);
			KeySlice *r = that->Key(r_idx, j);
			CopyPrefix(r, data_ + that->pre_len_, left);
			CopyKey(r, l, left, key_len_);
		}
	} else { // this->pre_len_ < that->pre_len_
		that->Expand(this->pre_len_);
		uint16_t slot_len = PageByte + that->key_len_;
		uint16_t limit = that->total_key_ * slot_len + that->pre_len_;
		for (uint16_t i = index, j = 0, end = index + move; i != end; ++i, ++j) {
			r_idx[j] = limit + j * slot_len;
			KeySlice *l = this->Key(l_idx, i);
			KeySlice *r = that->Key(r_idx, j);
			CopyKey(r, l, 0, key_len_);
		}
	}

	fence->page_no_ = that->page_no_;

	FillFrom(total_key_ - move);

	that->total_key_ += move;
	this->dirty_ = 1;
	that->dirty_ = 1;
	return true;
}

void Page::Combine(Page *left, Page *right, KeySlice *old_key, KeySlice *new_key,
	KeySlice *slice)
{
	pre_len_ = std::min(left->pre_len_, right->pre_len_);
	if (pre_len_) {
		assert(!memcmp(left->data_, right->data_, pre_len_));
		memcpy(this->data_, left->data_, pre_len_);
		key_len_ -= pre_len_;
		CalculateDegree(key_len_, pre_len_);
	}

	uint16_t *l_idx = left->Index();
	uint16_t *r_idx = right->Index();

	KeySlice *fence = left->Key(l_idx, left->total_key_ - 1);
	if (left->pre_len_) CopyPrefix(old_key, left->data_, left->pre_len_);
	CopyKey(old_key, fence, left->pre_len_, left->key_len_);
	old_key->page_no_ = right->page_no_;

	uint16_t l = left->total_key_ / 3;
	uint16_t r = right->total_key_ / 3 + 1;
	assert(degree_ > l + r);

	char *curr = data_ + pre_len_;
	uint16_t *index = Index();
	index -= l + r;
	uint16_t pre = left->pre_len_ - pre_len_;
	for (uint16_t i = left->total_key_ - l - 1; i < left->total_key_ - 1; ++i) {
		KeySlice *key = left->Key(l_idx, i);
		*index++ = curr - data_;
		memcpy(curr, &key->page_no_, PageByte);
		curr += PageByte;
		if (pre) {
			memcpy(curr, left->data_ + pre_len_, pre);
			curr += pre;
		}
		memcpy(curr, key->key_, left->key_len_);
		curr += left->key_len_;
	}

	fence = left->Key(l_idx, left->total_key_ - l - 1);
	fence->page_no_ = page_no_;
	if (left->pre_len_) CopyPrefix(slice, left->data_, left->pre_len_);
	CopyKey(slice, fence, left->pre_len_, left->key_len_);

	left->FillFrom(left->total_key_ - l);

	pre = right->pre_len_ - pre_len_;
	for (uint16_t i = 0; i < r; ++i) {
		KeySlice *key = right->Key(r_idx, i);
		*index++ = curr - data_;
		memcpy(curr, &key->page_no_, PageByte);
		curr += PageByte;
		if (pre) {
			memcpy(curr, right->data_ + pre_len_, pre);
			curr += pre;
		}
		memcpy(curr, key->key_, right->key_len_);
		curr += right->key_len_;
	}

	total_key_ = l + r--;

	uint16_t above = right->total_key_ - r;
	uint16_t limit = above * (PageByte + right->key_len_) + right->pre_len_, j = r;
	for (uint16_t i = 0; i < r && j < right->total_key_; ++i) {
		if (r_idx[i] < limit) {
			for (; j < right->total_key_; ++j) {
				if (r_idx[j] >= limit) {
					KeySlice *o = right->Key(r_idx, i);
					KeySlice *n = right->Key(r_idx, j);
					r_idx[j] = r_idx[i];
					CopyKey(o, n, 0, right->key_len_);
					++j;
					break;
				}
			}
		}
	}
	right->total_key_ = above;

	fence = Key(Index(), total_key_ - 1);
	fence->page_no_ = right->page_no_;
	if (pre_len_) CopyPrefix(new_key, data_, pre_len_);
	CopyKey(new_key, fence, pre_len_, key_len_);

	dirty_ = 1;
	left->dirty_ = 1;
	right->dirty_ = 1;
}

bool Page::Full() const
{
	return total_key_ == degree_;
}

bool Page::NeedSplit()
{
	if (!Full()) return false;
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

bool Page::FenceKeyLessEqual(const Page *that) const
{
	int f = 0;
	KeySlice *last  = this->Key(this->Index(), this->total_key_ - 1);
	KeySlice *first = that->Key(that->Index(), 0);
	if (this->pre_len_ == that->pre_len_) {
		if (this->pre_len_)
			f = memcmp(this->data_, that->data_, this->pre_len_);
		if (f < 0) return true;
		f = memcmp(last->key_, first->key_, this->key_len_);
	} else if (this->pre_len_ < that->pre_len_) {
		if (this->pre_len_)
			f = memcmp(this->data_, that->data_, this->pre_len_);
		if (f < 0) return true;
		uint16_t left = that->pre_len_ - this->pre_len_;
		f = memcmp(last->key_, that->data_ + this->pre_len_, left);
		if (f < 0) return true;
		f = memcmp(last->key_ + left, first->key_, that->key_len_);
	} else {
		if (that->pre_len_)
			f |= memcmp(this->data_, that->data_, that->pre_len_);
		if (f < 0) return true;
		uint16_t left = this->pre_len_ - that->pre_len_;
		f = memcmp(this->data_ + that->pre_len_, first->key_, left);
		if (f < 0) return true;
		f = memcmp(last->key_, first->key_ + left, this->key_len_);
	}
	return f <= 0 ? true : false;
}

bool Page::FenceKeyEqual(const Page *that) const
{
	int f = 0;
	KeySlice *last  = this->Key(this->Index(), this->total_key_ - 1);
	KeySlice *first = that->Key(that->Index(), 0);
	if (this->pre_len_ == that->pre_len_) {
		if (this->pre_len_)
			f |= memcmp(this->data_, that->data_, this->pre_len_);
		f |= memcmp(last->key_, first->key_, this->key_len_);
	} else if (this->pre_len_ < that->pre_len_) {
		if (this->pre_len_)
			f |= memcmp(this->data_, that->data_, this->pre_len_);
		uint16_t left = that->pre_len_ - this->pre_len_;
		f |= memcmp(last->key_, that->data_ + this->pre_len_, left);
		f |= memcmp(last->key_ + left, first->key_, that->key_len_);
	} else {
		if (that->pre_len_)
			f |= memcmp(this->data_, that->data_, that->pre_len_);
		uint16_t left = this->pre_len_ - that->pre_len_;
		f |= memcmp(this->data_ + that->pre_len_, first->key_, left);
		f |= memcmp(last->key_, first->key_ + left, this->key_len_);
	}
	return f == 0 ? true : false;
}

std::string Page::ToString(bool f, bool f2) const
{
	std::ostringstream os;
	os << "type: ";
	if (type_ == LEAF)   os << "leaf  ";
	if (type_ == BRANCH) os << "branch  ";
	if (type_ == ROOT)   os << "root  ";
	os << "no: " << page_no_ << "  ";
	os << "fir: " << first_ << "  ";
	os << "tot: " << total_key_ << "  ";
	os << "lvl: " << (int)level_ << "  ";
	os << "keylen: " << (int)key_len_ << "  ";
	os << "deg: " << degree_ << "\n";

	if (pre_len_) {
		os << "pre_len: " << (int)pre_len_ << " ";
		os << "prefix: " << std::string(data_, pre_len_) << "\n";
	}

	uint16_t *index = Index();
	if (!f) {
		os << Key(index, 0)->ToString(key_len_);
		os << Key(index, total_key_ - 1)->ToString(key_len_);
	} else {
		// for (uint16_t i = 0; i != total_key_; ++i)
			// os << index[i] << " ";
		// os << "\n";
		for (uint16_t i = 0; i != total_key_; ++i) {
			KeySlice *key = Key(index, i);
			if (f2)
				os << key->page_no_ << " ";
			os << key->ToString(key_len_);
		}
	}
	os << "\nnext: " << Key(index, total_key_-1)->page_no_ << "\n";
	return os.str();
}

} // namespace Mushroom
