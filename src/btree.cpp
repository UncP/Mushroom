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

	char buf[BTreePage::DataId + key_len_] = {0};
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
	// 	Output(leaf);
	// 	getchar();
	// }
	++num;
	// if (++num == 73855) {
	// 	Traverse(-1);
	// 	Traverse(1);
	// 	Output(leaf);
	// 	Output(key);
	// 	leaf = pager_->GetPage(228);
	// 	Output(leaf);
	// 	exit(-1);
	// }
	if (!leaf->Insert(key)) {
		std::cout << "repeat\n";
		goto end;
	}
	// if (num == 73855) {
		// Output(leaf);
		// leaf = pager_->GetPage(leaf->Next());
		// Output(leaf);
	// }
	if (leaf->KeyNo() < degree_) goto end;
	// if (leaf->PageNo() == 228 && leaf->KeyNo() ==255) {
	// 	std::cout << leaf;
	// }

	Split(leaf, stack, depth);
	goto end;

end:
	if (leaf->Occupy())
		leaf->SetOccupy(false);
	while (depth > 1) {
		leaf = stack[--depth];
		// if (leaf->Occupy())
		leaf->SetOccupy(false);
	}
	return Success;
}

Status BTree::SplitRoot()
{
	BTreePage *new_root = pager_->NewPage(BTreePage::ROOT, root_->KeyLen(), root_->Level() + 1);
	BTreePage *next = pager_->NewPage(root_->Level() ? BTreePage::BRANCH : BTreePage::LEAF,
		root_->KeyLen(), root_->Level());
	// Output(root_);
	char key[BTreePage::PageByte + key_len_] = {0};
	KeySlice *slice = (KeySlice *)key;
	memset(slice->Data(), 0xFF, key_len_);
	new_root->Insert(slice);
	// Output(root_);
	root_->Split(next, slice);

	root_->AssignType(next->Type());
	root_->AssignPageNo(new_root->PageNo());
	new_root->AssignFirst(root_->PageNo());
	new_root->Insert(slice);
	// Output(new_root);
	// Output(root_);
	// Output(next);
	pager_->UnPinPage(new_root);
	pager_->PinPage(root_);
	new_root->AssignPageNo(0);

	root_ = new_root;

	// Output(root_);
	// Traverse(1);
	return Success;
}

Status BTree::Split(BTreePage *left, BTreePage **stack, uint8_t depth)
{
	BTreePage *right = nullptr, *parent = nullptr;
	char key[BTreePage::PageByte + key_len_];
	KeySlice *slice = (KeySlice *)key;

	if (left->Type() != BTreePage::ROOT) {
		right = pager_->NewPage(BTreePage::LEAF, left->KeyLen(), left->Level());
		assert(right);
		left->Split(right, slice);
		assert(depth > 0);
		parent = stack[--depth];
		// if (left->PageNo() == 228) {
		// 	std::cout << left;
		// 	std::cout << parent;
		// 	std::cout << num << std::endl;
		// }
		assert(parent);
		// if (left->PageNo() == 228) {
			// Output(parent);
			// std::cout << slice->PageNo() << std::endl;
			// Output(slice, 1);
		// }
		parent->Insert(slice);
		// if (left->PageNo() == 228) {
		// 	Output(left);
		// 	Output(right);
		// 	Output(parent);
		// 	std::cout << num << std::endl;
		// }
		// Output(parent);
	} else {
		assert(!depth);
		return SplitRoot();
	}

	for (; parent->KeyNo() == degree_;) {
		left = parent;
		right = pager_->NewPage(BTreePage::BRANCH, left->KeyLen(), left->Level());
		if (left->Type() != BTreePage::ROOT) {
			assert(depth > 0);
			parent = stack[--depth];
			assert(parent);
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
	assert(leaf);
	return leaf->Search(key, page_no);
}

bool BTree::Next(KeySlice *key, page_id *page_no) const
{
	BTreePage *leaf = *page_no ? pager_->GetPage(*page_no) : root_;
	bool flag = leaf->FindGreatEq(key, page_no);
	if (flag) return true;
	if (!*page_no) return false;
	// Output(leaf);
	leaf = pager_->GetPage(*page_no);
	assert(leaf);
	return leaf->FindGreatEq(key, page_no);
}

void BTree::Traverse(int level) const
{
	if (level > root_->Level())
		return ;
	if (level == -1)
		level = root_->Level();

	BTreePage *page = root_;
	page_id page_no;
	while (page->Level() != level) {
		page_no = page->First();
		page = pager_->GetPage(page_no);
	}
	for (;;) {
		// Output(page);
		// std::cout << page->PageNo() << "  ";
		// if (page->PageNo() == 452 || page->PageNo() == 228) {
		// 	Output(page);
		// 	std::cout << "!";
		// }
		page_id page_no = page->Next();
		if (!page_no) break;
		page = pager_->GetPage(page_no);
		assert(page->Level() == level);
	}
}

} // namespace Mushroom
