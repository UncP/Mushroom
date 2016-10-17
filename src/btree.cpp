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
#include "utility.hpp"

namespace Mushroom {

std::string BTree::ToString() const
{
	std::string str;
	char no[16];
	snprintf(no, 16, "%d  ", degree_);
	str = "阶: " + std::string(no);
	snprintf(no, 16, "%d  ", key_len_);
	str += "key_len: " + std::string(no) + "\n";
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

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;
	memset(key->Data(), 0xFF, key_len_);
	root_->Insert(key);

	return Success;
}

Status BTree::Close()
{
	// assert(root_->Write(pager_->fd()));
	if (pager_) assert(pager_->Close());
	return Success;
}

BTreePage* BTree::DescendToLeaf(const KeySlice *key, BTreePage **stack, uint8_t *depth) const
{
	BTreePage *parent = root_, *child = nullptr;
	for (; parent->Level(); ++*depth) {
		page_id page_no = parent->Descend(key);
		assert(child = pager_->GetPage(page_no));
		assert(child->Level() != parent->Level());
		stack[*depth] = parent;
		parent = child;
		parent->SetOccupy(true);
	}
	return parent;
}

Status BTree::Put(const KeySlice *key)
{
	uint8_t depth = 0;
	BTreePage* stack[8];

	BTreePage *leaf = DescendToLeaf(key, stack, &depth);
	if (!leaf->Insert(key)) {
		std::cout << "key existed ;)\n";
		goto end;
	}
	if (leaf->KeyNo() < degree_) goto end;
	Split(leaf, stack, depth);
	goto end;

end:
	if (leaf->Occupy())
		leaf->SetOccupy(false);
	while (depth > 1)
		stack[--depth]->SetOccupy(false);
	return Success;
}

bool BTree::Get(KeySlice *key) const
{
	uint8_t depth = 0;
	BTreePage* stack[8];
	BTreePage *leaf = DescendToLeaf(key, stack, &depth);
	bool flag = leaf->Search(key);

	if (leaf->Occupy())
		leaf->SetOccupy(false);
	while (depth > 1)
		stack[--depth]->SetOccupy(false);

	return flag;
}

Status BTree::SplitRoot()
{
	BTreePage *new_root = BTreePage::NewPage(0, BTreePage::ROOT, root_->KeyLen(),
		root_->Level() + 1);
	BTreePage *next = pager_->NewPage(root_->Level() ? BTreePage::BRANCH : BTreePage::LEAF,
	 	root_->KeyLen(), root_->Level());
	char key[BTreePage::PageByte + key_len_] = {0};
	KeySlice *slice = (KeySlice *)key;
	memset(slice->Data(), 0xFF, key_len_);
	new_root->Insert(slice);
	root_->Split(next, slice);
	root_->AssignType(next->Type());
	root_->AssignPageNo(pager_->IncrPageNo());
	new_root->AssignFirst(root_->PageNo());
	new_root->Insert(slice);
	pager_->PinPage(root_);
	root_ = new_root;
	return Success;
}

Status BTree::Split(BTreePage *left, BTreePage **stack, uint8_t depth)
{
	BTreePage *right = nullptr, *parent = nullptr;
	char key[BTreePage::PageByte + key_len_];
	KeySlice *slice = (KeySlice *)key;

	if (left->Type() != BTreePage::ROOT) {
		right = pager_->NewPage(BTreePage::LEAF, left->KeyLen(), left->Level());
		left->Split(right, slice);
		parent = stack[--depth];
		parent->Insert(slice);
	} else {
		return SplitRoot();
	}

	for (; parent->KeyNo() == degree_;) {
		left = parent;
		right = pager_->NewPage(BTreePage::BRANCH, left->KeyLen(), left->Level());
		if (left->Type() != BTreePage::ROOT) {
			parent = stack[--depth];
		} else {
			return SplitRoot();
		}
		left->Split(right, slice);
		parent->Insert(slice);
	}
	return Success;
}

BTreePage* BTree::First(page_id *page_no, int level) const
{
	if (level > root_->Level())
		return nullptr;
	if (level == -1)
		level = root_->Level();

	BTreePage *page = root_;
	for (; page->Level() != level;)
		page = pager_->GetPage(page->First());
	if (page_no)
		*page_no = page->PageNo();
	return page;
}

bool BTree::Next(KeySlice *key, page_id *page_no, uint16_t *index) const
{
	BTreePage *leaf = *page_no ? pager_->GetPage(*page_no) : root_;
	bool flag = leaf->Ascend(key, page_no, index);
	if (flag)
		return true;
	if (!*page_no)
		return false;
	leaf = pager_->GetPage(*page_no);
	return leaf->Ascend(key, page_no, index);
}

void BTree::Traverse(int level) const
{
	BTreePage *page = First(nullptr, level);
	if (!page) return ;
	for (;;) {
		Output(page);
		page_id page_no = page->Next();
		if (!page_no) break;
		page = pager_->GetPage(page_no);
		assert(page->Level() == level);
	}
}

} // namespace Mushroom
