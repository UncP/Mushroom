/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:12:13
**/

#include <cassert>
#include <sstream>

#include "btree.hpp"
#include "utility.hpp"

namespace Mushroom {

BTree::BTree(const int fd, const int key_len)
{
	key_len_ = static_cast<uint8_t>(key_len);

	degree_ = BTreePage::CalculateDegree(key_len);

	latch_manager_ = new LatchManager();
	assert(latch_manager_);

	root_ = BTreePage::NewPage(BTreePage::ROOT, key_len_, 0, degree_);
	assert(root_);
	BTreePage::SetZero((uint64_t)root_);

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;
	memset(key->Data(), 0xFF, key_len_);
	page_id next;
	assert(root_->Insert(key, next) == InsertOk);
}

BTree::~BTree()
{
	delete [] (char *)BTreePage::ZERO;
	delete latch_manager_;
}

Status BTree::Close()
{
	printf("total page: %u\n", BTreePage::current);
	return Success;
}

Latch* BTree::DescendToLeaf(const KeySlice *key, page_id *stack, uint8_t *depth) const
{
	Latch *latch = latch_manager_->GetLatch(root_->page_no_);
	latch->LockShared();
	for (; latch->page_->level_;) {
		page_id page_no = latch->page_->Descend(key);
		page_id pre_no = latch->page_->page_no_;
		uint8_t pre_le = latch->page_->level_;
		latch->UnlockShared();
		latch = latch_manager_->GetLatch(page_no);
		latch->LockShared();
		if (latch->page_->level_ != pre_le) {
			stack[*depth] = pre_no;
			++*depth;
		}
	}
	return latch;
}

void BTree::Insert(Latch **latch, KeySlice *key)
{
	InsertStatus status;
	page_id next = 0;
	for (; (status = (*latch)->page_->Insert(key, next));) {
		switch (status) {
			case MoveRight: {
				Latch *pre = *latch;
				(*latch) = latch_manager_->GetLatch(next);
				(*latch)->Lock();
				pre->Unlock();
				next = 0;
				break;
			}
			default: {
				printf("existed key :(\n");
				assert(0);
			}
		}
	}
}

Status BTree::Put(KeySlice *key)
{
	uint8_t depth = 0;
	page_id stack[8];

	auto latch = DescendToLeaf(key, stack, &depth);
	latch->Upgrade();

	Insert(&latch, key);

	BTreePage *left = latch->page_;
	for (; left->NeedSplit(); ) {
		if (left->type_ != BTreePage::ROOT) {
			BTreePage *right = BTreePage::NewPage(left->type_, left->key_len_, left->level_,
        left->degree_);
			left->Split(right, key);
			Latch *pre = latch;
			assert(depth != 0);
			page_id page_no = stack[--depth];
			latch = latch_manager_->GetLatch(page_no);
			latch->Lock();
			Insert(&latch, key);
			pre->Unlock();
			left = latch->page_;
		} else {
			SplitRoot(latch);
			break;
		}
	}
	latch->Unlock();
	return Success;
}

Status BTree::SplitRoot(Latch *latch)
{
	auto level = root_->level_;
	BTreePage *left = BTreePage::NewPage(BTreePage::ROOT, key_len_, level+1, degree_);
	BTreePage *right = BTreePage::NewPage(level ? BTreePage::BRANCH : BTreePage::LEAF,
	 	root_->key_len_, level, degree_);

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *slice = (KeySlice *)buf;

	root_->type_ = level ? BTreePage::BRANCH : BTreePage::LEAF;
	root_->Split(right, slice);

	char tmp[BTreePage::PageByte + key_len_] = {0};
	KeySlice *limit = (KeySlice *)tmp;
	memset(limit->Data(), 0xFF, key_len_);

	page_id page_no = root_->page_no_;
	left->AssignFirst(page_no);
	assert(left->Insert(limit, page_no) == InsertOk);
	assert(left->Insert(slice, page_no) == InsertOk);
	__sync_bool_compare_and_swap(&root_, root_, left);
	return Success;
}

Status BTree::Get(KeySlice *key) const
{
	uint8_t depth = 0;
	page_id stack[8];

	auto latch = DescendToLeaf(key, stack, &depth);
	bool flag = latch->page_->Search(key);
	if (!flag) {
		printf("%s", key->ToString(key_len_).c_str());
		Output(latch->page_);
	}
	latch->UnlockShared();
	return flag ? Success : Fail;
}

BTreePage* BTree::First(page_id *page_no, int level) const
{
	if (level > root_->level_)
		return nullptr;
	if (level == -1)
		level = root_->level_;

	BTreePage *page = root_;
	for (; page->level_ != level;)
		page = BTreePage::GetPage(page->first_);

	if (page_no)
		*page_no = page->page_no_;
	return page;
}

bool BTree::Next(KeySlice *key, page_id *page_no, uint16_t *index) const
{
	BTreePage *leaf = BTreePage::GetPage(*page_no);

	bool flag = leaf->Ascend(key, page_no, index);
	if (flag) return true;
	if (!*page_no) return false;

	leaf = BTreePage::GetPage(*page_no);

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

		page = BTreePage::GetPage(page_no);

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

std::string BTree::ToString() const
{
	std::ostringstream os;
	os << "阶: "<< degree_ << " " << "key_len: " << key_len_ << std::endl;
	return os.str();
}

} // namespace Mushroom
