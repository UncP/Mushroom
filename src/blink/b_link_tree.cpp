/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-07 20:12:13
**/

#include <cassert>

#include "b_link_tree.hpp"
#include "page.hpp"
#include "pool_manager.hpp"

namespace Mushroom {

BLinkTree::BLinkTree(uint32_t key_len)
:pool_manager_(new PoolManager()), root_(0), key_len_(key_len)
{
	degree_ = Page::CalculateDegree(key_len_);
	Set set;
	set.page_ = pool_manager_->NewPage(Page::ROOT, key_len_, 0, degree_);
	set.page_->InsertInfiniteKey();
	set.page_->AdjustBloomFilter(100);
}

BLinkTree::~BLinkTree()
{
	delete pool_manager_;
}

void BLinkTree::Free()
{
	printf("%s", pool_manager_->GetPage(root_.get())->Status().c_str());
	pool_manager_->Free();
}

void BLinkTree::DescendToLeaf(const KeySlice *key, Set &set, LockType type)
{
	set.page_no_ = root_.get();
	set.page_ = pool_manager_->GetPage(set.page_no_);
	uint8_t level = set.page_->level_;
	for (; level;) {
		set.page_->LockShared();
		page_t pre_no = set.page_->page_no_;
		set.page_no_ = set.page_->Descend(key);
		set.page_->UnlockShared();
		set.page_ = pool_manager_->GetPage(set.page_no_);
		if (set.page_->level_ != level) {
			set.stack_[set.depth_++] = pre_no;
			--level;
		}
	}
	if (type == WriteLock)
		set.page_->Lock();
	else
		set.page_->LockShared();
}

bool BLinkTree::Split(Set &set, KeySlice *key)
{
	if (set.page_->type_ != Page::ROOT) {
		Page *right = pool_manager_->NewPage(set.page_->type_, set.page_->key_len_,
			set.page_->level_, set.page_->degree_);
		set.page_->Split(right, key);
		set.page_->Unlock();
		assert(set.depth_);
		set.page_no_ = set.stack_[--set.depth_];
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.page_->Lock();
		Insert(set, key);
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
		new_root->Insert(key, page_no);
		root_ = new_root->page_no_;
		return false;
	}
}

void BLinkTree::Insert(Set &set, KeySlice *key)
{
	InsertStatus status;
	for (; (status = set.page_->Insert(key, set.page_no_));) {
		assert(status == MoveRight);
		Latch *pre = set.page_->GetLatch();
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.page_->Lock();
		pre->Unlock();
	}
}

bool BLinkTree::Put(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set, WriteLock);

	Insert(set, key);

	for (; set.page_->NeedSplit() && Split(set, key); )
		continue;

	set.page_->Unlock();
	return true;
}

bool BLinkTree::Get(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set, ReadLock);

	for (uint16_t idx = 0; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_) {
			set.page_->UnlockShared();
			assert(0);
			return false;
		}
		set.page_no_ = set.page_->Next();
		Latch *pre = set.page_->GetLatch();
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.page_->LockShared();
		pre->Unlock();
	}

	set.page_->UnlockShared();
	return true;
}

void BLinkTree::CheckBloomFilter()
{
	Page *page = pool_manager_->GetPage(root_.get());
	while (page->level_) {
		page_t next_level = page->first_;
		assert(page->filter_ == 0);
		while (page->Next()) {
			page = pool_manager_->GetPage(page->Next());
			assert(page->filter_ == 0);
		}
		page = pool_manager_->GetPage(next_level);
	}
	assert((page->total_key_ / 100 + 1) * 100 == page->filter_);
	while (page->Next()) {
		page = pool_manager_->GetPage(page->Next());
		assert((page->total_key_ / 100 + 1) * 100 == page->filter_);
	}
}

} // namespace Mushroom
