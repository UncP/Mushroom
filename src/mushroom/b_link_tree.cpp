/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-07 20:12:13
**/

#include <cassert>

#include "latch.hpp"
#include "b_link_tree.hpp"
#include "page.hpp"
#include "latch_manager.hpp"
#include "pool_manager.hpp"

namespace Mushroom {

BLinkTree::BLinkTree(const char *name, uint32_t key_len)
:latch_manager_(new LatchManager()), pool_manager_(new PoolManager(name)), root_(0),
key_len_(key_len)
{
	degree_ = Page::CalculateDegree(key_len_);
	Set set;
	set.page_ = pool_manager_->NewPage(Page::ROOT, key_len_, 0, degree_);
	set.page_->InsertInfiniteKey();
}

BLinkTree::~BLinkTree()
{
	delete latch_manager_;
	delete pool_manager_;
}

void BLinkTree::Free()
{
	pool_manager_->Free();
}

void BLinkTree::FlushDirtyPages()
{
	pool_manager_->Flush(latch_manager_);
}

void BLinkTree::DescendToLeaf(const KeySlice *key, Set &set)
{
	set.page_no_ = root_.get();
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	set.page_ = pool_manager_->GetPage(set.page_no_);
	set.latch_->LockShared();
	for (; set.page_->level_;) {
		page_t  pre_no = set.page_->page_no_;
		uint8_t pre_le = set.page_->level_;
		set.page_no_ = set.page_->Descend(key);
		set.latch_->UnlockShared();
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.latch_->LockShared();
		if (set.page_->level_ != pre_le)
			set.stack_[set.depth_++] = pre_no;
	}
}

bool BLinkTree::SplitAndPromote(Set &set, KeySlice *key)
{
	if (set.page_->type_ != Page::ROOT) {
		Page *right = pool_manager_->NewPage(set.page_->type_, set.page_->key_len_,
			set.page_->level_, set.page_->degree_);
		set.page_->Split(right, key);
		Latch *pre = set.latch_;
		assert(set.depth_ != 0);
		set.page_no_ = set.stack_[--set.depth_];
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.latch_->Lock();
		Insert(set, key);
		pre->Unlock();
		return true;
	} else {
		uint8_t level = set.page_->level_;
		Page *new_root = pool_manager_->NewPage(Page::ROOT, key_len_, level + 1, degree_);
		Page *right = pool_manager_->NewPage(level ? Page::BRANCH : Page::LEAF,
			set.page_->key_len_, level, degree_);

		new_root->InsertInfiniteKey();
		new_root->AssignFirst(set.page_->page_no_);

		set.page_->type_ = level ? Page::BRANCH : Page::LEAF;
		set.page_->Split(right, key);

		page_t page_no = 0;
		assert(new_root->Insert(key, page_no) == InsertOk);
		root_ = new_root->page_no_;
		return false;
	}
}

void BLinkTree::Insert(Set &set, KeySlice *key)
{
	InsertStatus status;
	for (; (status = set.page_->Insert(key, set.page_no_));) {
		switch (status) {
			case MoveRight: {
				Latch *pre = set.latch_;
				set.latch_ = latch_manager_->GetLatch(set.page_no_);
				set.page_ = pool_manager_->GetPage(set.page_no_);
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

bool BLinkTree::Put(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set);

	set.latch_->Upgrade();

	Insert(set, key);

	for (; set.page_->NeedSplit() && SplitAndPromote(set, key); )
		continue;

	set.latch_->Unlock();
	return true;
}

bool BLinkTree::Get(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set);

	for (uint16_t idx = 0; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_) {
			set.latch_->UnlockShared();
			return false;
		}
		set.page_no_ = set.page_->Next();
		Latch *pre = set.latch_;
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.latch_->LockShared();
		pre->UnlockShared();
	}

	set.latch_->UnlockShared();
	return true;
}

void BLinkTree::LoadLeaf(const KeySlice *key, Set &set)
{
	DescendToLeaf(key, set);

	uint16_t idx = 0;
	for (; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_)
			break;
		set.page_no_ = set.page_->Next();
		Latch *pre = set.latch_;
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.latch_->LockShared();
		pre->UnlockShared();
	}

	set.latch_->Upgrade();
}

Page* BLinkTree::Split(Set &set, KeySlice *key)
{
	Page *right;
	if (set.page_->type_ != Page::ROOT) {
		Page *right = pool_manager_->NewPage(set.page_->type_, set.page_->key_len_,
			set.page_->level_, set.page_->degree_);
		set.page_->Split(right, key);
	} else {
		uint8_t level = set.page_->level_;
		Page *new_root = pool_manager_->NewPage(Page::ROOT, key_len_, level + 1, degree_);
		Page *right = pool_manager_->NewPage(level ? Page::BRANCH : Page::LEAF,
			set.page_->key_len_, level, degree_);

		new_root->InsertInfiniteKey();
		new_root->AssignFirst(set.page_->page_no_);

		set.page_->type_ = level ? Page::BRANCH : Page::LEAF;
		set.page_->Split(right, key);
		set.stack_[set.depth_++] = new_root->page_no_;
	}
	return right;
}

bool BLinkTree::BatchPut(Page *page)
{
	uint16_t  total = page->total_key_;
	uint16_t *index = page->Index();
	Set *set = new Set[total];
	for (uint16_t i = 0; i < total; ++i) {
		KeySlice *key = page->Key(index, i);
		if (i) {
			Page *pre = set[i-1].page_;
			uint16_t idx;
			if (pre->Search(key, &idx) || idx != pre->total_key_) {
				set[i].page_ = 0;
			} else {
				// Deadlock ???
				LoadLeaf(key, set[i]);
			}
		} else {
			LoadLeaf(key, set[i]);
		}
	}

	uint16_t k = total;
	for (int32_t i = total - 1; i >= 0; --i) {
		if (!set[i].page_) continue;

		uint8_t ptr = 0;
		Page   *pages[3]; // new pages will not be more then 2
		TempSlice(tmp1, key_len_);
		TempSlice(tmp2, key_len_);
		pages[ptr++] = set[i].page_;
		for (uint16_t j = i; j < k; ++j) {
			KeySlice *key = page->Key(index, j);
			Page *cur;
			for (uint8_t m = 0; m < ptr; ++m) {
				cur = pages[m];
				uint16_t idx;
				if (cur->Search(key, &idx) || idx != cur->total_key_)
					break;
			}
			page_t page_no;
			assert(cur->Insert(key, page_no) != MoveRight);
			if (cur->NeedSplit()) {
				set[i].page_ = cur;
				if (ptr == 1)
					pages[ptr++] = Split(set[i], tmp1);
				else
					pages[ptr++] = Split(set[i], tmp2);
			}
		}

		k = i;

		if (ptr == 1) { // no split
			set.latch_->Unlock();
		} else {
			Latch *pre = set.latch_; // this latch is write locked
			assert(set[i].depth_);
			page_t parent = set[i].stack_[--set[i].depth_];
			for (int8_t j = --ptr; j >= 0 ; --j) {
				set.latch_ = latch_manager_->GetLatch(parent);
				set.page_ = pool_manager_->GetPage(parent);
				set.latch_->Lock();
				if (j == 1)
					Insert(set, tmp1);
				else
					Insert(set, tmp2);
				set.latch_->Unlock();
			}
			pre->Unlock();
		}
	}

	delete [] set;
	return true;
}

} // namespace Mushroom
