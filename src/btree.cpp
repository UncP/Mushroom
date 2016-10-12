/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-07 20:12:13
**/

#include <cassert>

#include "btree.hpp"

namespace Mushroom {

std::string BTree::ToString() const
{
	std::string str;
	char no[16];
	snprintf(no, 16, "%d  ", degree_);
	str = "阶: " + std::string(no);
	snprintf(no, 16, "%d  ", min_key_);
	str = "min_key: " + std::string(no);
	snprintf(no, 16, "%d  ", max_key_);
	str = "max_key: " + std::string(no);
	snprintf(no, 16, "%d  ", min_node_);
	str = "min_node: " + std::string(no);
	snprintf(no, 16, "%d  ", max_node_);
	str = "max_node: " + std::string(no) + "\n";
	return std::move(str);
}

Status BTree::Init(const int fd, const int key_len)
{
	assert(key_len >= 4 && key_len <= 256);

	key_len_ = static_cast<uint8_t>(key_len);

	BTreePage *page = nullptr;
	uint16_t offset = (uint16_t)((char *)page->Data() - (char *)page);
	degree_ = static_cast<uint16_t>(
		(BTreePage::PageSize - offset) / (BTreePage::DataId + BTreePage::IndexByte + key_len));

	std::cout << degree_ << std::endl;

	min_key_  = ((degree_ - 1) + ((degree_ - 1) % 2)) >> 1;
	max_key_  = degree_ - 1;

	min_node_ = (degree_ + (degree_ % 2)) >> 1;
	max_node_ = degree_ + 1;

	pager_ = new BTreePager(fd);

	return Success;
}

Status BTree::Close()
{
	if (pager_) assert(pager_->Close());
	return Success;
}

BTreePage* BTree::DescendToLeaf(const Slice &key, BTreePage **stack, uint8_t *depth)
{
	BTreePage *node = root_;
	for (; node->Type() != BTreePage::LEAF; ++*depth) {
		page_id page_no = node->Descend(key);
		assert(page_no);
		stack[*depth] = node;
		assert(node = pager_->GetPage(page_no));
	}
	return node;
}

Status BTree::Put(const Slice &key, const Slice &val)
{
	assert(key.Length() == key_len_);
	assert(val.Empty());

	uint8_t depth = 0;
	BTreePage* stack[8];

	BTreePage *leaf = DescendToLeaf(key, stack, &depth);
	if (!leaf->Insert(key)) return Success;

	if (leaf->KeyNo() < degree_) return Success;

	Split(leaf, stack, depth);

	return Success;
}

Status BTree::Split(BTreePage *left, BTreePage **stack, uint8_t depth)
{
	BTreePage *right = pager_->NewPage(BTreePage::LEAF, key_len_), *parent = nullptr;
	left->Split(right);
	if (left->Type() != BTreePage::ROOT) {
		parent = stack[--depth];
	} else {
		assert(!depth);
		root_ = parent = pager_->NewPage(BTreePage::ROOT, key_len_);
		left->AssignType(BTreePage::LEAF);
		parent->AssignFirst(left->PageNo());
	}
	page_id page_no = right->PageNo();
	char key[key_len_ + BTreePage::PageByte];
	memcpy(key, &page_no, BTreePage::PageByte);
	memcpy(key + BTreePage::PageByte, right->Data() + BTreePage::PageByte, key_len_);
	parent->Insert((const KeySlice *)key);
	for (; parent->KeyNo() == degree_;) {
		left = parent;
		right = pager_->NewPage(BTreePage::BRANCH, key_len_);
		if (left->Type() != BTreePage::ROOT) {
			parent = stack[--depth];
		} else {
			assert(!depth);
			root_ = parent = pager_->NewPage(BTreePage::ROOT, key_len_);
			parent->AssignFirst(left->PageNo());
		}
		page_no = right->PageNo();
		memcpy(key, &page_no, BTreePage::PageByte);
		left->Split(right, key + BTreePage::PageByte);
		parent->Insert((const KeySlice *)key);
	}
	return Success;
}

} // namespace Mushroom
