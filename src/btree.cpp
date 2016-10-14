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
	snprintf(no, 16, "%d  ", key_len_);
	str += "key_len: " + std::string(no) + "\n";
	root_->Info();
	return std::move(str);
}

Status BTree::Init(const int fd, const int key_len)
{
	assert(key_len <= 256);

	key_len_ = static_cast<uint8_t>(key_len);

	BTreePage *page = nullptr;
	uint16_t offset = (uint16_t)((char *)page->Data() - (char *)page);
	degree_ = static_cast<uint16_t>(
		(BTreePage::PageSize - offset) / (BTreePage::DataId + BTreePage::IndexByte + key_len));

	pager_ = new BTreePager(fd);

	root_ = BTreePage::NewPage(0, BTreePage::ROOT, key_len_, 0);
	assert(root_ && pager_);

	char buf[BTreePage::DataId + key_len_];
	KeySlice *key = (KeySlice *)buf;
	memset(key, 0xFF, sizeof(buf));
	root_->Insert(key);

	return Success;
}

Status BTree::Close()
{
	if (pager_) assert(pager_->Close());
	return Success;
}

BTreePage* BTree::DescendToLeaf(const KeySlice *key, BTreePage **stack, uint8_t *depth) const
{
	BTreePage *parent = root_, *child = nullptr;
	for (; parent->Level();) {
		page_id page_no = parent->Descend(key);
		assert(child = pager_->GetPage(page_no));
		assert(child->Level() != parent->Level());
		stack[*depth++] = parent;
		parent = child;
	}
	return parent;
}

Status BTree::Put(const KeySlice *key)
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
	BTreePage *next = pager_->NewPage(root_->Level() ? BTreePage::BRANCH : BTreePage::LEAF,
		root_->KeyLen(), root_->Level());
	char key[key_len_ + BTreePage::PageByte];
	KeySlice *slice = (KeySlice *)key;
	root_->Split(next, slice);
	new_root->AssignFirst(root_->PageNo());
	new_root->Insert(slice);
	root_->AssignType(next->Type());
	pager_->UnPinPage(new_root);
	root_->AssignPageNo(new_root->PageNo());
	new_root->AssignPageNo(0);
	pager_->PinPage(root_);
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

bool BTree::Get(KeySlice *key, page_id *page_no) const
{
	uint8_t depth = 0;
	BTreePage* stack[8];
	BTreePage *leaf = DescendToLeaf(key, stack, &depth);
	return leaf->Search(key, page_no);
}

bool BTree::Next(KeySlice *key, page_id *page_no) const
{
	BTreePage *leaf = *page_no ? pager_->GetPage(*page_no) : root_;
	bool flag = leaf->FindGreatEq(key, page_no);
	if (flag) return true;
	if (*page_no) {
		leaf = pager_->GetPage(*page_no);
		return leaf->FindGreatEq(key, page_no);
	}
	return false;
}

} // namespace Mushroom
