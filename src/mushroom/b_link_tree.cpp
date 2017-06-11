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

BLinkTree::BLinkTree(uint32_t key_len)
:latch_manager_(new LatchManager()), pool_manager_(new PoolManager()), root_(0),
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
	Audit();
	pool_manager_->Free();
}

void BLinkTree::GetParent(Set &set)
{
	assert(set.depth_);
	set.page_no_ = set.stack_[--set.depth_];
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	set.page_ = pool_manager_->GetPage(set.page_no_);
	set.latch_->Lock();
}

void BLinkTree::GetNext(Set &set)
{
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	set.page_ = pool_manager_->GetPage(set.page_no_);
	set.latch_->Lock();
}

void BLinkTree::DescendToLeaf(const KeySlice *key, Set &set, LockType type)
{
	set.page_no_ = root_.get();
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	set.page_ = pool_manager_->GetPage(set.page_no_);
	uint8_t level = set.page_->level_;
	for (; level;) {
		set.latch_->LockShared();
		page_t pre_no = set.page_->page_no_;
		set.page_no_ = set.page_->Descend(key);
		set.latch_->UnlockShared();
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		if (set.page_->level_ != level) {
			set.stack_[set.depth_++] = pre_no;
			--level;
		}
	}
	if (type == WriteLock)
		set.latch_->Lock();
	else
		set.latch_->LockShared();
	assert(!set.page_->level_);
}

bool BLinkTree::SplitAndPromote(Set &set, KeySlice *key)
{
	if (set.page_->type_ != Page::ROOT) {
		Page *right = pool_manager_->NewPage(set.page_->type_, set.page_->key_len_,
			set.page_->level_, set.page_->degree_);
		set.page_->Split(right, key);
		set.latch_->Unlock();
		GetParent(set);
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
		assert(new_root->Insert(key, page_no) == InsertOk);
		root_ = new_root->page_no_;
		return false;
	}
}

void BLinkTree::Insert(Set &set, KeySlice *key)
{
	InsertStatus status;
	for (; (status = set.page_->Insert(key, set.page_no_));) {
		assert(status == MoveRight);
		Latch *pre = set.latch_;
		GetNext(set);
		pre->Unlock();
	}
}

bool BLinkTree::Update(Set &set, KeySlice *old_key, KeySlice *new_key)
{
	UpdateStatus status;
	for (; (status = set.page_->Update(old_key, new_key, set.page_no_));) {
		if (status == MoveNext) {
			Latch *pre = set.latch_;
			GetNext(set);
			pre->Unlock();
		} else {
			return true;
		}
	}
	return false;
}

bool BLinkTree::Put(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set, WriteLock);

	Insert(set, key);

	if (set.page_->NeedSplit()) {
		page_t page_no = set.page_->Next();
		if (page_no) {
			Latch *latch = latch_manager_->GetLatch(page_no);
			Page *next = pool_manager_->GetPage(page_no);
			latch->Lock();
			TempSlice(tmp, key_len_);
			if (set.page_->Move(next, tmp, key)) {
				latch->Unlock();
				Latch *pre = set.latch_;
				GetParent(set);
				Update(set, tmp, key);
				pre->Unlock(); // crucial
			} else {
				Page *right = pool_manager_->NewPage(set.page_->type_, key_len_,
					set.page_->level_, degree_);
				TempSlice(tmp2, key_len_);
				right->Combine(set.page_, next, tmp, tmp2, key);
				latch->Unlock();
				uint8_t depth = set.depth_;
				Latch *pre = set.latch_;
				GetParent(set);
				page_t parent = set.page_no_;
				for (; Update(set, tmp, tmp2) && set.depth_;) {
					pre->Unlock();
					pre = set.latch_;
					GetParent(set);
				}
				pre->Unlock();
				if (set.page_->page_no_ != parent) {
					set.depth_ = depth;
					set.latch_->Unlock();
					GetParent(set);
				}
				Insert(set, key);
				for (; set.page_->NeedSplit() && SplitAndPromote(set, key); )
					continue;
			}
		} else { // last leaf
			for (; set.page_->NeedSplit() && SplitAndPromote(set, key); )
				continue;
		}
	}

	set.latch_->Unlock();
	return true;
}

bool BLinkTree::Get(KeySlice *key)
{
	Set set;

	DescendToLeaf(key, set, ReadLock);

	for (uint16_t idx = 0; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_) {
			set.latch_->UnlockShared();
			printf("%s\n", set.page_->ToString(true, true).c_str());
			printf("%s", key->ToString(key_len_).c_str());
			assert(0);
			return false;
		}
		set.page_no_ = set.page_->Next();
		Latch *pre = set.latch_;
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.latch_->LockShared();
		pre->UnlockShared();
	}

	assert(!key->page_no_);
	set.latch_->UnlockShared();
	return true;
}

void BLinkTree::LoadLeaf(const KeySlice *key, Set &set)
{
	DescendToLeaf(key, set, WriteLock);

	uint16_t idx = 0;
	for (; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_)
			break;
		set.page_no_ = set.page_->Next();
		Latch *pre = set.latch_;
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		set.page_ = pool_manager_->GetPage(set.page_no_);
		set.latch_->Lock();
		pre->Unlock();
	}
}

Page* BLinkTree::Split(Set &set, KeySlice *key)
{
	Page *right;
	if (set.page_->type_ != Page::ROOT) {
		right = pool_manager_->NewPage(set.page_->type_, set.page_->key_len_,
			set.page_->level_, set.page_->degree_);
		set.page_->Split(right, key);
	} else {
		assert(!set.depth_);
		uint8_t level = set.page_->level_;
		Page *new_root = pool_manager_->NewPage(Page::ROOT, key_len_, level + 1, degree_);
		right = pool_manager_->NewPage(level ? Page::BRANCH : Page::LEAF,
			set.page_->key_len_, level, degree_);

		new_root->InsertInfiniteKey();
		new_root->AssignFirst(set.page_->page_no_);

		set.page_->type_ = level ? Page::BRANCH : Page::LEAF;
		set.page_->Split(right, key);
		set.stack_[set.depth_++] = new_root->page_no_;
		root_ = new_root->page_no_;
	}
	return right;
}

void BLinkTree::Audit()
{
	Page *root = pool_manager_->GetPage(root_.get());
	ShowPage(root_.get());
	int level = root->level_ - 1;
	for (; level >= 0; --level) {
		Page *page = pool_manager_->GetPage(root->first_);
		for (; page->Next();) {
			Page *pre = page;
			Page *cur = pool_manager_->GetPage(pre->Next());
			assert(pre->level_ == cur->level_);
			if (pre->level_)
				printf("%s\n", pre->ToString(true, true).c_str());
			assert(pre->FenceKeyLessEqual(cur));
			page = cur;
		}
	}
}

void BLinkTree::ShowPage(page_t page_no)
{
	printf("%s\n", pool_manager_->GetPage(page_no)->ToString(true, true).c_str());
}

} // namespace Mushroom
