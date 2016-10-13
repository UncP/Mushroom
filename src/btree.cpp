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

	char key[key_len_ + BTreePage::PageByte];
	memset(key, 0, BTreePage::PageByte);
	memset(key + BTreePage::PageByte, 1, key_len_);
	KeySlice *slice = (KeySlice *)key;
	root_->Insert(slice);

	pager_ = new BTreePager(fd);

	return Success;
}

Status BTree::Close()
{
	if (pager_) assert(pager_->Close());
	return Success;
}

BTreePage* BTree::DescendToLeaf(const KeySlice *key, BTreePage **stack, uint8_t *depth)
{
	BTreePage *node = root_;
	for (; node->Type() != BTreePage::LEAF; ++*depth) {
		page_id page_no = node->Descend(key);
		stack[*depth] = node;
		assert(node = pager_->GetPage(page_no));
	}
	return node;
}

Status BTree::Put(const KeySlice *key, const DataSlice *val)
{
	uint8_t depth = 0;
	BTreePage* stack[8];

	BTreePage *leaf = DescendToLeaf(key, stack, &depth);
	if (!leaf->Insert(key)) return Success;

	if (leaf->KeyNo() < degree_) return Success;

	Split(leaf, stack, depth);

	return Success;
}

Status BTree::SplitRoot()
{
	BTreePage *new_root = pager_->NewPage(BTreePage::ROOT, root_->KeyLen(), root_->Level() + 1);
	BTreePage *next = pager_->NewPage(root_->Type(), root_->KeyLen(), root_->Level());
	char key[key_len_ + BTreePage::PageByte];
	KeySlice *slice = (KeySlice *)key;
	root_->Split(next, slice);
	new_root->Insert(slice);
	root_->AssignType(root_->Level() ? BTreePage::BRANCH : BTreePage::LEAF);
	new_root->AssignFirst(root_->PageNo());
	root_->AssignPageNo(new_root->PageNo());
	new_root->AssignPageNo(0);
	root_ = new_root;
	return Success;
}

Status BTree::Split(BTreePage *left, BTreePage **stack, uint8_t depth)
{
	BTreePage *right = nullptr, *parent = nullptr;
	char key[key_len_ + BTreePage::PageByte];
	KeySlice *slice = (KeySlice *)key;

	if (left->Type() != BTreePage::ROOT) {
		right = pager_->NewPage(BTreePage::LEAF, left->KeyLen(), left->Level());
		left->Split(right, slice);
		parent = stack[--depth];
		parent->Insert(slice);
	} else {
		assert(!depth);
		return SplitRoot();
	}

	for (; parent->KeyNo() == degree_;) {
		left = parent;
		right = pager_->NewPage(BTreePage::BRANCH, left->KeyLen(), left->Level());
		if (left->Type() != BTreePage::ROOT) {
			parent = stack[--depth];
		} else {
			assert(!depth);
			return SplitRoot();
		}
		left->Split(right, slice);
		parent->Insert(slice);
	}
	return Success;
}

} // namespace Mushroom
