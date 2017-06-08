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

bool BLinkTree::operator==(const BLinkTree &that) const
{
	return *pool_manager_ == *that.pool_manager_;
}

void BLinkTree::Free()
{
	// Set set;
	// set.page_no_ = root_.get();
	// for (; set.page_no_; ) {
	// 	set.page_ = pool_manager_->GetPage(set.page_no_);
	// 	set.page_no_ = set.page_->first_;
	// }
	// for (;;) {
	// 	set.page_ = pool_manager_->GetPage(set.page_no_);
	// 	printf("%s\n", set.page_->ToString(true, true).c_str());
	// 	set.page_no_ = set.page_->Next();
	// 	if (!set.page_no_) break;
	// }
	pool_manager_->Free();
}

void BLinkTree::FlushDirtyPages()
{
	pool_manager_->Flush(latch_manager_);
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
		// if (!memcmp(key->key_, "NiplGIeQ2Vc1XPVf", 16)) {
			// printf("%s\n", set.page_->ToString(true, false).c_str());
			// printf("%s\n", right->ToString(true, false).c_str());
		// }
		GetParent(set);
		// if (!memcmp(key->key_, "NiplGIeQ2Vc1XPVf", 16)) {
		// 	printf("%s\n", set.page_->ToString(true, false).c_str());
		// }
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
		// if (status != MoveRight) {
			// printf("%s\n", set.page_->ToString(true, false).c_str());
			// printf("%u %s", key->page_no_, key->ToString(key_len_).c_str());
		// }
		assert(status == MoveRight);
		Latch *pre = set.latch_;
		GetNext(set);
		pre->Unlock();
	}
}

void BLinkTree::Update(Set &set, KeySlice *old_key, KeySlice *new_key)
{
	for (; !set.page_->Update(old_key, new_key, set.page_no_);) {
		Latch *pre = set.latch_;
		GetNext(set);
		pre->Unlock();
	}
}

bool BLinkTree::Put(KeySlice *key)
{
	TempSlice(copy, key_len_);
	CopyKey(copy, key, 0, key_len_);
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
				// latch = set.latch_;
				// printf("move %u %u\n", set.page_no_, page_no);
				// printf("%s\n", set.page_->ToString(false).c_str());
				// printf("%s\n", next->ToString(false).c_str());
				/*if (!memcmp(key->key_, "NiplGIeQ2Vc1XPVf", 16)) {
					printf("%s\n", set.page_->ToString(true, false).c_str());
					printf("%s\n", next->ToString(true, false).c_str());
				} else */if (!memcmp(tmp->key_, "NiplGIeQ2Vc1XPVf", 16)) {
					printf("%s\n", set.page_->ToString(true, true).c_str());
					printf("%s\n", next->ToString(true, true).c_str());
				}
				GetParent(set);
				// if (!memcmp(key->key_, "NiplGIeQ2Vc1XPVf", 16)) {
					// printf("%s\n", set.page_->ToString(true, false).c_str());
				// }
				Update(set, tmp, key);
				/*if (!memcmp(key->key_, "NiplGIeQ2Vc1XPVf", 16)) {
					printf("%s\n", set.page_->ToString(true, false).c_str());
				} else */if (!memcmp(tmp->key_, "NiplGIeQ2Vc1XPVf", 16)) {
					printf("%s\n", set.page_->ToString(true, true).c_str());
					Page *page = pool_manager_->GetPage(set.page_->Next());
					printf("%s\n", page->ToString(true, true).c_str());
					page = pool_manager_->GetPage(root_.get());
					printf("%s\n", page->ToString(true, true).c_str());
					printf("%s", copy->ToString(key_len_).c_str());
					page = pool_manager_->GetPage(1631);
					printf("%s\n", page->ToString(true, true).c_str());
				}
				pre->Unlock();
			} else {
				latch->Unlock();
				// Page *right = pool_manager_->NewPage(set.page_->type_, key_len_,
				// 	set.page_->level_, degree_);
				// // printf("combine %u %u %u\n", set.page_no_, right->page_no_, page_no);
				// TempSlice(tmp2, key_len_);
				// right->Combine(set.page_, next, tmp, tmp2, key);
				// // latch->Unlock();
				// Latch *pre = set.latch_;
				// // latch = set.latch_;
				// GetParent(set);
				// page_t parent = set.page_no_;
				// Update(set, tmp, tmp2);
				// latch->Unlock();
				// pre->Unlock();
				// if (set.page_->page_no_ != parent) {
				// 	set.latch_->Unlock();
				// 	set.stack_[set.depth_++] = parent;
				// 	GetParent(set);
				// }
				// Insert(set, key);
				for (; set.page_->NeedSplit() && SplitAndPromote(set, key); )
					continue;
			}
		} else {
			for (; set.page_->NeedSplit() && SplitAndPromote(set, key); )
				continue;
		}
	}

	// for (; set.page_->NeedSplit() && SplitAndPromote(set, key); )
		// continue;

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
			// printf("%s\n", set.page_->ToString(true, true).c_str());
			// printf("%s", key->ToString(key_len_).c_str());
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

	char buf1[sizeof(page_t) + key_len_];
	char buf2[sizeof(page_t) + key_len_];
	KeySlice *mid[2];
	mid[0] = (KeySlice *)buf1;
	mid[1] = (KeySlice *)buf2;
	uint16_t k = total;
	for (int32_t i = total - 1; i >= 0; --i) {
		if (!set[i].page_) continue;

		uint8_t ptr = 0;
		Page   *pages[3]; // new pages will not be more then 2
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
				pages[ptr] = Split(set[i], mid[ptr-1]);
				++ptr;
			}
		}

		k = i;

		if (ptr == 1) {
			set[i].latch_->Unlock();
		} else {
			Latch *pre = set[i].latch_;
			assert(set[i].depth_);
			page_t parent = set[i].stack_[--set[i].depth_];
			for (int8_t j = ptr-1; j >= 1; --j) {
				set[i].latch_ = latch_manager_->GetLatch(parent);
				set[i].page_  = pool_manager_->GetPage(parent);
				set[i].latch_->Lock();
				Insert(set[i], mid[j-1]);
				uint8_t pre_depth = set[i].depth_;
				for (; set[i].page_->NeedSplit() && SplitAndPromote(set[i], mid[j-1]);)
					continue;
				set[i].depth_ = pre_depth;
				set[i].latch_->Unlock();
			}
			pre->Unlock();
		}
	}
	delete [] set;
	return true;
}

} // namespace Mushroom
