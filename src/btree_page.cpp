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
	if (pre_len_) {
		int res = ComparePrefix(key, data_, pre_len_);
		if (res < 0) {
			*idx = 0;
			return false;
		} else if (res > 0) {
			*idx = --high;
			*slice = Key(index, high);
			return false;
		}
	}
	KeySlice *curr = nullptr;
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
	if (high) *slice = Key(index, high - 1);
	return false;
}

page_id BTreePage::Descend(const KeySlice *key) const
{
	uint16_t index;
	KeySlice *slice = nullptr;
	Traverse(key, &index, &slice, 0);
	return index ? slice->PageNo() : first_;
}

bool BTreePage::Search(KeySlice *key) const
{
	uint16_t index;
	KeySlice *slice = nullptr;
	return Traverse(key, &index, &slice);
}

InsertStatus BTreePage::Insert(const KeySlice *key, page_id &page_no)
{
	uint16_t pos;
	KeySlice *slice = nullptr;
	bool flag = Traverse(key, &pos, &slice);
	if (flag) return ExistedKey;
	if (pos == total_key_ && pos) {
		page_no = Next();
		assert(page_no);
		return MoveRight;
	}
	// if (!pos && pre_len_ && memcmp(key->Data(), data_, pre_len_))
	// 	return NeedExpand;

	uint16_t end = total_key_ * (PageByte + key_len_) + pre_len_;
	page_id num = key->PageNo();
	memcpy(data_ + end, &num, PageByte);
	memcpy(data_ + end + PageByte, key->Data() + pre_len_, key_len_);

	uint16_t *index = Index();
	--index;
	if (pos) memmove(&index[0], &index[1], pos << 1);
	index[pos] = end;
	++total_key_;
	dirty_ = true;
	return InsertOk;
}

bool BTreePage::Ascend(KeySlice *key, page_id *page_no, uint16_t *idx)
{
	uint16_t *index = Index();
	if (*idx < (total_key_ - 1)) {
		if (pre_len_)
			CopyPrefix(key, data_, pre_len_);
		CopyKey(key, Key(index, *idx), pre_len_, key_len_);
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
	KeySlice *fence = (KeySlice *)(this->data_ + l_idx[left++]);

	if (pre_len_) {
		memcpy(that->data_, this->data_, pre_len_);
		that->pre_len_ = this->pre_len_;
		CopyPrefix(slice, data_, pre_len_);
	}

	slice->AssignPageNo(that->PageNo());
	memcpy(slice->Data() + pre_len_, fence->Data(), key_len_);

	if (level_) {
		that->AssignFirst(fence->PageNo());
		memcpy(fence->Data(), Key(l_idx, left)->Data(), key_len_);
		r_idx -= --right;
		++index;
	} else {
		r_idx -= right;
	}

	fence->AssignPageNo(that->PageNo());

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
	this->dirty_ = true;
	that->dirty_ = true;
}

void BTreePage::Insert(BTreePage *that, KeySlice *key)
{
	assert(pre_len_);
	char buf[PageByte + key_len_ + pre_len_];
	KeySlice *slice = (KeySlice *)buf;
	CopyKey(slice, key, 0, key_len_ + pre_len_);
	Split(that, key);
	char prefix[pre_len_];
	memcpy(prefix, data_, pre_len_);
	char tmp[PageSize];
	BTreePage *copy = (BTreePage *)tmp;
	memcpy(copy, this, PageSize);
	uint16_t *index = Index();
	char *curr = data_;
	--index;
	*index++ = 0;
	memcpy(curr, slice, PageByte + key_len_ + pre_len_);
	curr += PageByte + key_len_ + pre_len_;
	uint16_t *cindex = copy->Index();
	for (uint16_t i = 0; i != total_key_; ++i, ++index) {
		*index = curr - data_;
		KeySlice *k = Key(cindex, i);
		page_id page_no = k->PageNo();
		memcpy(curr, &page_no, PageByte);
		curr += PageByte;
		memcpy(curr, prefix, pre_len_);
		curr += pre_len_;
		memcpy(curr, k->Data(), key_len_);
		curr += key_len_;
	}
	key_len_ += pre_len_;
	pre_len_ = 0;
	degree_ = CalculateDegree(key_len_);
	++total_key_;
}

bool BTreePage::NeedSplit()
{
	if (total_key_ < degree_)
		return false;
	uint16_t *index = Index();
	const char *first = Key(index, 0)->Data();
	const char *last  = Key(index, total_key_ - 1)->Data();
	char prefix[key_len_];
	uint8_t pre_len = 0;
	for (; first[pre_len] == last[pre_len]; ++pre_len)
		prefix[pre_len] = first[pre_len];
	if (!pre_len)
		return true;
	uint16_t degree = CalculateDegree(key_len_, pre_len_);
	if (degree <= degree_)
		return true;
	char buf[PageSize];
	BTreePage *copy = (BTreePage *)buf;
	memcpy(copy, this, PageSize);
	memcpy(data_, prefix, pre_len);
	char *curr = data_ + pre_len;
	uint16_t *cindex = copy->Index();
	uint8_t suf_len = key_len_ - pre_len;
	for (uint16_t i = 0; i != total_key_; ++i, ++index) {
		KeySlice *key = copy->Key(cindex, i);
		*index = curr - data_;
		page_id page_no = key->PageNo();
		memcpy(curr, &page_no, PageByte);
		curr += PageByte;
		memcpy(curr, key->Data() + pre_len, suf_len);
		curr += suf_len;
	}
	pre_len_ = pre_len;
	key_len_ = suf_len;
	degree_  = degree;
	return false;
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

	if (pre_len_) {
		os << "pre_len: " << (int)pre_len_ << " ";
		os << "prefix: " << std::string(data_, pre_len_) << "\n";
	}

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
	std::string pre;
	for (uint16_t i = 0; i != total_key_; ++i) {
		KeySlice *key = (KeySlice *)(data_ + index[i]);
		std::string tmp(key->Data(), key_len_);
		if (pre.size() && pre >= tmp)
			std::cout << pre << " " << tmp << std::endl;
		if (keys.find(tmp) == keys.end())
			keys.insert(tmp);
		else
			std::cout << "repeated key " << tmp << std::endl;
		pre = tmp;
	}
}

} // namespace Mushroom
