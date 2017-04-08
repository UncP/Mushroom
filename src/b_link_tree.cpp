/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:12:13
**/

#include <cstring>
#include <cassert>

#include "b_link_tree.hpp"
#include "slice.hpp"
#include "page.hpp"
#ifndef NOLATCH
#include "latch_manager.hpp"
#endif
#include "pool_manager.hpp"

namespace Mushroom {

#ifndef NOLATCH
BLinkTree::BLinkTree(int key_len, LatchManager *latch_manager, PoolManager *page_manager)
:latch_manager_(latch_manager), page_manager_(page_manager), root_(0),
 key_len_((uint8_t)key_len)
{
	degree_ = Page::CalculateDegree(key_len_);
}
#else
BLinkTree::BLinkTree(int key_len, PoolManager *page_manager)
:page_manager_(page_manager), root_(0),
 key_len_((uint8_t)key_len)
{
	degree_ = Page::CalculateDegree(key_len_);
}
#endif

BLinkTree::~BLinkTree()
{
	#ifndef NOLATCH
	delete latch_manager_;
	#endif
	delete page_manager_;
}

void BLinkTree::Initialize()
{
	Set set;
	set.page_no_ = root_;
	#ifndef NOLATCH
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	assert(set.latch_->TryWriteLock());
	#endif
	set.page_ = page_manager_->NewPage(Page::ROOT, key_len_, 0, degree_);
	char buf[Page::PageByte + key_len_];
	memset(buf, 0, Page::PageByte + key_len_);
	KeySlice *key = (KeySlice *)buf;
	memset(key->Data(), 0xFF, key_len_);
	page_id next = 0;
	assert(set.page_->Insert(key, next) == InsertOk);
	#ifndef NOLATCH
	set.latch_->Unlock();
	#endif
}

bool BLinkTree::Free()
{
	printf("total page: %u\n", page_manager_->TotalPage());
	page_manager_->Free();
	return true;
}

void BLinkTree::DescendToLeaf(const KeySlice *key, Set &set) const
{
	set.page_no_ = root_;
	#ifndef NOLATCH
	set.latch_ = latch_manager_->GetLatch(set.page_no_);
	#endif
	set.page_ = page_manager_->GetPage(set.page_no_);
	#ifndef NOLATCH
	set.latch_->LockShared();
	#endif
	for (; set.page_->level_;) {
		page_id pre_no = set.page_->page_no_;
		uint8_t pre_le = set.page_->level_;
		set.page_no_ = set.page_->Descend(key);
		#ifndef NOLATCH
		set.latch_->UnlockShared();
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		#endif
		set.page_ = page_manager_->GetPage(set.page_no_);
		#ifndef NOLATCH
		set.latch_->LockShared();
		#endif
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
				#ifndef NOLATCH
				Latch *pre = set.latch_;
				set.latch_ = latch_manager_->GetLatch(set.page_no_);
				#endif
				set.page_ = page_manager_->GetPage(set.page_no_);
				#ifndef NOLATCH
				set.latch_->Lock();
				pre->Unlock();
				#endif
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
	#ifndef NOLATCH
	#ifdef LSM
	Ref();
	#endif
	#endif

	DescendToLeaf(key, set);

	#ifndef NOLATCH
	set.latch_->Upgrade();
	#endif

	Insert(set, key);

	for (; set.page_->NeedSplit(); ) {
		if (set.page_->type_ != Page::ROOT) {
			Page *right = page_manager_->NewPage(set.page_->type_, set.page_->key_len_,
				set.page_->level_, set.page_->degree_);
			set.page_->Split(right, key);
			#ifndef NOLATCH
			Latch *pre = set.latch_;
			#endif
			assert(set.depth_ != 0);
			set.page_no_ = set.stack_[--set.depth_];
			#ifndef NOLATCH
			set.latch_ = latch_manager_->GetLatch(set.page_no_);
			#endif
			set.page_ = page_manager_->GetPage(set.page_no_);
			#ifndef NOLATCH
			set.latch_->Lock();
			#endif
			Insert(set, key);
			#ifndef NOLATCH
			pre->Unlock();
			#endif
		} else {
			SplitRoot(set);
			break;
		}
	}
	#ifndef NOLATCH
	set.latch_->Unlock();
	#ifdef LSM
	Unref();
	#endif
	#endif
	return true;
}

void BLinkTree::SplitRoot(Set &set)
{
	uint8_t level = set.page_->level_;
	Page *new_root = page_manager_->NewPage(Page::ROOT, key_len_, level+1, degree_);
	Page *right = page_manager_->NewPage(level ? Page::BRANCH : Page::LEAF,
	 	set.page_->key_len_, level, degree_);

	char buf[Page::PageByte + key_len_];
	memset(buf, 0, Page::PageByte + key_len_);
	KeySlice *slice = (KeySlice *)buf;

	set.page_->type_ = level ? Page::BRANCH : Page::LEAF;
	set.page_->Split(right, slice);

	char tmp[Page::PageByte + key_len_];
	memset(tmp, 0, Page::PageByte + key_len_);
	KeySlice *limit = (KeySlice *)tmp;
	memset(limit->Data(), 0xFF, key_len_);

	new_root->AssignFirst(set.page_->page_no_);
	page_id page_no = 0;
	assert(new_root->Insert(limit, page_no) == InsertOk);
	assert(new_root->Insert(slice, page_no) == InsertOk);
	__sync_val_compare_and_swap(&root_, root_, new_root->page_no_);
}

bool BLinkTree::Get(KeySlice *key) const
{
	Set set;

	DescendToLeaf(key, set);

	for (uint16_t idx = 0; !set.page_->Search(key, &idx);) {
		if (idx != set.page_->total_key_) {
			#ifndef NOLATCH
			set.latch_->UnlockShared();
			#endif
			assert(0);
			return false;
		}
		set.page_no_ = set.page_->Next();
		#ifndef NOLATCH
		Latch *pre = set.latch_;
		set.latch_ = latch_manager_->GetLatch(set.page_no_);
		#endif
		set.page_ = page_manager_->GetPage(set.page_no_);
		#ifndef NOLATCH
		set.latch_->LockShared();
		pre->UnlockShared();
		#endif
	}

	#ifndef NOLATCH
	set.latch_->UnlockShared();
	#endif
	return true;
}

bool BLinkTree::First(Page **page, int32_t level) const
{
	*page = page_manager_->GetPage(root_);
	if (level > (*page)->level_)
		return false;
	if (level == -1)
		level = (*page)->level_;

	for (; (*page)->level_ != level;)
		*page = page_manager_->GetPage((*page)->first_);

	return true;
}

bool BLinkTree::Next(KeySlice *key, Page **page, uint16_t *index) const
{
	page_id page_no;
	if ((*page)->Ascend(key, &page_no, index))
		return true;
	if (page_no) {
		*page = page_manager_->GetPage(page_no);
		return (*page)->Ascend(key, &page_no, index);
	}
	return false;
}

BLinkTree::Iterator::Iterator(const BLinkTree *b_link_tree, int32_t level)
:b_link_tree_(b_link_tree), level_(level), index_(0) {
	char *buf = new char[MAX_KEY_LENGTH + Page::PageByte];
	key_ = (KeySlice *)buf;
	memset(key_->Data(), 0, BLinkTree::MAX_KEY_LENGTH);
}

BLinkTree::Iterator::~Iterator() { delete [] key_; }

inline bool BLinkTree::Iterator::Begin() { return b_link_tree_->First(&curr_, level_); }

inline bool BLinkTree::Iterator::Next() { return b_link_tree_->Next(key_, &curr_, &index_); }

#ifdef LSM
inline bool BLinkTree::NeedCompact() const { return page_manager_->ReachMax(); }

bool BLinkTree::Clear()
{
	#ifndef NOLATCH
	if (!mutex_.TryLock())
		return false;
	while (ref_) cond_.Wait(&mutex_);
	mutex_.Unlock();
	assert(!ref_);
	#endif
	return true;
}
#endif

} // namespace Mushroom
