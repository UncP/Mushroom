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

BLinkTree::BLinkTree(uint32_t key_len):key_len_((uint8_t)key_len)
{
	latch_manager_ = new LatchManager();
	pool_manager_ = new PoolManager();
	degree_ = Page::CalculateDegree(key_len_);
	Initialize();
}

void BLinkTree::Initialize()
{
	root_ = 0;

	Set set;
	set.page_ = pool_manager_->NewPage(Page::ROOT, key_len_, 0, degree_);
	set.page_->InsertInfiniteKey();
}

BLinkTree::~BLinkTree()
{
	delete latch_manager_;
	delete pool_manager_;
}

bool BLinkTree::Free()
{
	pool_manager_->Free();
	return true;
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

	for (; set.page_->NeedSplit(); ) {
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
		} else {
			SplitRoot(set);
			break;
		}
	}
	set.latch_->Unlock();
	return true;
}

void BLinkTree::SplitRoot(Set &set)
{
	uint8_t level = set.page_->level_;
	Page *new_root = pool_manager_->NewPage(Page::ROOT, key_len_, level + 1, degree_);
	Page *right = pool_manager_->NewPage(level ? Page::BRANCH : Page::LEAF,
		set.page_->key_len_, level, degree_);

	new_root->InsertInfiniteKey();
	new_root->AssignFirst(set.page_->page_no_);

	TempSlice(slice, key_len_);

	set.page_->type_ = level ? Page::BRANCH : Page::LEAF;
	set.page_->Split(right, slice);

	page_t page_no = 0;
	assert(new_root->Insert(slice, page_no) == InsertOk);
	root_ = new_root->page_no_;
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

} // namespace Mushroom
