/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-07 20:12:13
**/

#include <cassert>
#include <sstream>
#include <thread>
#include <chrono>

#include "btree.hpp"
#include "utility.hpp"

namespace Mushroom {

BTree::BTree(const int fd, const int key_len)
{
	key_len_ = static_cast<uint8_t>(key_len);

	degree_ = BTreePage::CalculateDegree(key_len);

	latch_manager_ = new LatchManager();

	btree_pager_ = new BTreePager(fd);

	root_ = btree_pager_->NewPage(BTreePage::ROOT, key_len_, 0, degree_, false);

	assert(latch_manager_ && root_ && btree_pager_);

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;
	memset(key->Data(), 0xFF, key_len_);
	page_id next;
	root_->Insert(key, next);
}

Status BTree::Close()
{
	// assert(root_->Write(btree_pager_->fd()));
	if (btree_pager_) assert(btree_pager_->Close());
	return Success;
}

std::pair<BTreePage*, Latch*> BTree::DescendToLeaf(const KeySlice *key, page_id *stack,
	uint8_t *depth) const
{
	Latch *latch = latch_manager_->GetLatch(0);
	latch->LockShared();
	BTreePage *parent = root_, *child = nullptr;
	for (; parent->Level();) {
		page_id page_no = parent->Descend(key);
		latch->UnlockShared();
		latch = latch_manager_->GetLatch(page_no);
		latch->LockShared();
		assert(child = btree_pager_->GetPage(page_no));
		if (child->Level() != parent->Level()) {
			stack[*depth] = parent->PageNo();
			++*depth;
		}
		parent = child;
	}
	return {parent, latch};
}

Status BTree::InsertKey(BTreePage *page, Latch *latch, const KeySlice *key)
{
	InsertStatus status;
	page_id next = 0;
	for (; status = page->Insert(key, next); ) {
		switch (status) {
			case MoveRight:
				Latch *pre = latch;
				latch = latch_manager_->GetLatch(next);
				latch->Lock();
				pre->Unlock();
				page = btree_pager_->GetPage(next);
				next = 0;
				break;
			case NeedSplit;
				new_page = btree_pager_->NewPage(page->Type(), root_->KeyLen(), left->Level(), degree_);
				page->SplitInsert(new_page, key);
				break;
			case ExistedKey:
				latch->Unlock();
				std::cout << "key existed ;)\n";
				return Fail;
		}
	}
	return Success;
}

Status BTree::Put(const KeySlice *key)
{
	uint8_t depth = 0;
	page_id stack[8];

	std::pair<BTreePage*, Latch*> pair = DescendToLeaf(key, stack, &depth);
	BTreePage *leaf = pair.first;
	Latch *latch = pair.second;

	latch->Upgrade();

	assert(InsertKey(leaf, latch, key));

	if (!leaf->NeedSplit()) {
		latch->Unlock();
		return Success;
	}

	Split(leaf, latch, stack, depth);
	return Success;
}

Status BTree::Get(KeySlice *key) const
{
	uint8_t depth = 0;
	page_id stack[8];

	std::pair<BTreePage*, Latch*> pair = DescendToLeaf(key, stack, &depth);
	bool flag = pair.first->Search(key);
	pair.second->UnlockShared();
	if (!flag) {
		std::cout << pair.first->ToString();
		std::cout << key->ToString() << std::endl;
		pair.first->Analyze();
	}
	return flag ? Success : Fail;
}

Status BTree::SplitRoot()
{
	BTreePage *new_root = btree_pager_->NewPage(BTreePage::ROOT, root_->KeyLen(),
		root_->Level() + 1, degree_, false);
	BTreePage *next = btree_pager_->NewPage(root_->Level() ? BTreePage::BRANCH : BTreePage::LEAF,
	 	root_->KeyLen(), root_->Level(), degree_);

	char key[BTreePage::PageByte + key_len_] = {0};
	KeySlice *slice = (KeySlice *)key;
	memset(slice->Data(), 0xFF, key_len_);
	page_id page_no = 0;
	new_root->Insert(slice, page_no);

	root_->Split(next, slice);
	root_->AssignType(next->Type());
	root_->AssignPageNo(new_root->PageNo());
	new_root->AssignFirst(root_->PageNo());
	new_root->AssignPageNo(0);
	new_root->Insert(slice, page_no);
	btree_pager_->PinPage(root_);
	root_ = new_root;
	return Success;
}

Status BTree::Split(BTreePage *left, Latch *latch, page_id *stack, uint8_t depth)
{
	BTreePage *right = nullptr, *parent = nullptr;
	char key[BTreePage::PageByte + key_len_];
	KeySlice *slice = (KeySlice *)key;

	for (; left->NeedSplit(); ) {
		if (left->Type() != BTreePage::ROOT) {
			right = btree_pager_->NewPage(left->Type(), left->KeyLen(), left->Level(), degree_);
			left->Split(right, slice);
			latch->Downgrade();
			Latch *pre = latch;
			page_id page_no = stack[--depth];
			latch = latch_manager_->GetLatch(page_no);
			latch->Lock();
			if (page_no)
				parent = btree_pager_->GetPage(page_no);
			else
				parent = root_;
			assert(InsertKey(parent, latch, slice));
			pre->UnlockShared();
			left = parent;
		} else {
			SplitRoot();
			break;
		}
	}
	latch->Unlock();
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
		page = btree_pager_->GetPage(page->First());
	if (page_no)
		*page_no = page->PageNo();
	return page;
}

bool BTree::Next(KeySlice *key, page_id *page_no, uint16_t *index) const
{
	BTreePage *leaf = *page_no ? btree_pager_->GetPage(*page_no) : root_;
	bool flag = leaf->Ascend(key, page_no, index);
	if (flag) return true;
	if (!*page_no) return false;
	leaf = btree_pager_->GetPage(*page_no);
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
		page = btree_pager_->GetPage(page_no);
		assert(page->Level() == level);
	}
}

bool BTree::KeyCheck(std::ifstream &in, int total) const
{
	std::string val;
	char buf[BTreePage::PageByte + key_len_] = { 0 };
	KeySlice *key = (KeySlice *)buf;

	in.seekg(0);
	for (int i = 0; !in.eof() && i != total; ++i) {
		in >> val;
		memcpy(key->Data(), val.c_str(), key_len_);
		if (!Get(key)) {
			std::cout << key->ToString() << std::endl;
			return false;
		}
	}
	return true;
}

std::string BTree::ToString() const
{
	std::ostringstream os;
	os << "阶: "<< degree_ << " " << "key_len: " << key_len_ << std::endl;
	return os.str();
}

} // namespace Mushroom
