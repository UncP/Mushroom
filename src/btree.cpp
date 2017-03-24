/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:12:13
**/

#include <iostream>
#include <cassert>
#include <sstream>

#include "btree.hpp"

namespace Mushroom {

BTree::BTree(int key_len, LatchManager *latch_manager, PageManager *page_manager,
	page_id *root)
:latch_manager_(latch_manager), page_manager_(page_manager), root_(root),
 key_len_((uint8_t)key_len)
{
	degree_ = BTreePage::CalculateDegree(key_len_);
}

void BTree::Initialize()
{
	Set set;
	set.page_no_ = *root_;
	assert(!set.page_no_);
	set.latch_ = latch_manager_->GetLatch(*root_);
	assert(set.latch_->TryWriteLock());
	set.page_ = page_manager_->NewPage(BTreePage::ROOT, key_len_, 0, degree_);
	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;
	memset(key->Data(), 0xFF, key_len_);
	page_id next = 0;
	assert(set.page_->Insert(key, next) == InsertOk);
	set.latch_->Unlock();
}

BTree::~BTree()
{
	delete page_manager_;
}

bool BTree::Free()
{
	printf("total page: %u\n", page_manager_->Total());
	page_manager_->Free();
	return true;
}

void BTree::DescendToLeaf(const KeySlice *key, Set &set) const
{
	set.page_no_ = *root_;
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	set.page_ = page_manager_->GetPage(set.page_no_);
	set.latch_->LockShared();
	for (; set.page_->level_;) {
		page_id pre_no = set.page_->page_no_;
		uint8_t pre_le = set.page_->level_;
		set.page_no_ = set.page_->Descend(key);
		set.latch_->UnlockShared();
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = page_manager_->GetPage(set.page_no_);
		set.latch_->LockShared();
		if (set.page_->level_ != pre_le)
			set.stack_[set.depth_++] = pre_no;
	}
}

void BTree::Insert(Set &set, KeySlice *key)
{
	InsertStatus status;
	for (; (status = set.page_->Insert(key, set.page_no_));) {
		switch (status) {
			case MoveRight: {
				Latch *pre = set.latch_;
				set.latch_ = latch_manager_->GetLatch(set.page_no_);
				set.page_ = page_manager_->GetPage(set.page_no_);
				set.latch_->Lock();
				pre->Unlock();
				break;
			}
			default: {
				printf("existed key :(\n");
				assert(0);
			}
		}
	}
}

bool BTree::Put(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set);
	set.latch_->Upgrade();

	Insert(set, key);

	for (; set.page_->NeedSplit(); ) {
		if (set.page_->type_ != BTreePage::ROOT) {
			BTreePage *right = page_manager_->NewPage(set.page_->type_, set.page_->key_len_,
				set.page_->level_, set.page_->degree_);
			set.page_->Split(right, key);
			Latch *pre = set.latch_;
			assert(set.depth_ != 0);
			set.page_no_ = set.stack_[--set.depth_];
			set.latch_ = latch_manager_->GetLatch(set.page_no_);
			set.page_ = page_manager_->GetPage(set.page_no_);
			set.latch_->Lock();
			Insert(set, key);
			pre->Unlock();
		} else {
			SplitRoot(set);
			break;
		}
	}
	set.latch_->Unlock();
	return true;
}

void BTree::SplitRoot(Set &set)
{
	uint8_t level = set.page_->level_;
	BTreePage *new_root = page_manager_->NewPage(BTreePage::ROOT, key_len_, level+1, degree_);
	BTreePage *right = page_manager_->NewPage(level ? BTreePage::BRANCH : BTreePage::LEAF,
	 	set.page_->key_len_, level, degree_);

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *slice = (KeySlice *)buf;

	set.page_->type_ = level ? BTreePage::BRANCH : BTreePage::LEAF;
	set.page_->Split(right, slice);

	char tmp[BTreePage::PageByte + key_len_] = {0};
	KeySlice *limit = (KeySlice *)tmp;
	memset(limit->Data(), 0xFF, key_len_);

	new_root->AssignFirst(set.page_->page_no_);
	page_id page_no = 0;
	assert(new_root->Insert(limit, page_no) == InsertOk);
	assert(new_root->Insert(slice, page_no) == InsertOk);
	__sync_val_compare_and_swap(root_, *root_, new_root->page_no_);
}

bool BTree::Get(KeySlice *key) const
{
	Set set;

	DescendToLeaf(key, set);

	for (uint16_t idx = 0; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_) {
			set.latch_->UnlockShared();
			printf("%s", key->ToString(key_len_).c_str());
			set.page_->Analyze();
			std::cout << set.page_->ToString();
			return false;
		}
		set.page_no_ = set.page_->Next();
		Latch *pre = set.latch_;
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = page_manager_->GetPage(set.page_no_);
		set.latch_->LockShared();
		pre->UnlockShared();
	}

	set.latch_->UnlockShared();
	return true;
}

BTreePage* BTree::First(page_id *page_no, int level) const
{
	BTreePage *page = page_manager_->GetPage(*root_);
	if (level > page->level_)
		return nullptr;
	if (level == -1)
		level = page->level_;

	for (; page->level_ != level;)
		page = page_manager_->GetPage(page->first_);

	if (page_no)
		*page_no = page->page_no_;
	return page;
}

bool BTree::Next(KeySlice *key, page_id *page_no, uint16_t *index) const
{
	BTreePage *leaf = page_manager_->GetPage(*page_no);

	bool flag = leaf->Ascend(key, page_no, index);
	if (flag) return true;
	if (!*page_no) return false;

	leaf = page_manager_->GetPage(*page_no);

	return leaf->Ascend(key, page_no, index);
}

void BTree::Traverse(int level) const
{
	BTreePage *page = First(nullptr, level);
	if (!page) return ;
	for (;;) {
		std::cout << page->ToString();
		page_id page_no = page->Next();
		if (!page_no) break;

		page = page_manager_->GetPage(page_no);

		assert(page->level_ == level);
	}
}

bool BTree::KeyCheck(std::ifstream &in, int total) const
{
	std::string val;
	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;

	for (int i = 0; !in.eof() && i != total; ++i) {
		in >> val;
		memcpy(key->Data(), val.c_str(), key_len_);
		if (!Get(key)) return false;
	}
	return true;
}

} // namespace Mushroom
