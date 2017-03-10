/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:12:13
**/

#include <cassert>
#include <sstream>
#include <thread>
#include <chrono>

#include "btree.hpp"
#include "utility.hpp"

namespace Mushroom {

BTree::BTree(const int fd, const int key_len):inserted_(0)
{
	key_len_ = static_cast<uint8_t>(key_len);

	degree_ = BTreePage::CalculateDegree(key_len);

	latch_manager_ = new LatchManager();

	BTreePage *root = BTreePage::NewPage(BTreePage::ROOT, key_len_, 0, degree_);
	BTreePage::SetZero((uint64_t)root);

	assert(latch_manager_);

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;
	memset(key->Data(), 0xFF, key_len_);
	page_id next;
	root->Insert(key, next);
}

Status BTree::Close()
{
	return Success;
}

Latch* BTree::DescendToLeaf(const KeySlice *key, page_id *stack, uint8_t *depth) const
{
	Latch *latch = latch_manager_->GetLatch(0);
	latch->LockShared();
	for (; latch->page_->Level();) {
		page_id page_no = latch->page_->Descend(key);
		// std::cout << page_no << std::endl;
		assert(page_no != 0);
		page_id pre_no = latch->page_->PageNo();
		uint8_t pre_le = latch->page_->Level();
		latch->UnlockShared();
		latch = latch_manager_->GetLatch(page_no);
		latch->LockShared();
		if (latch->page_->Level() != pre_le) {
			stack[*depth] = pre_no;
			++*depth;
		}
	}
	return latch;
}

bool BTree::Insert(Latch **latch, KeySlice *key)
{
	InsertStatus status;
	page_id next = 0;
	for (; (status = (*latch)->page_->Insert(key, next));) {
		switch (status) {
			case MoveRight: {
				Latch *pre = *latch;
				*latch = latch_manager_->GetLatch(next);
				(*latch)->Lock();
				pre->Unlock();
				next = 0;
				break;
			}
			default: {
				// std::cout << (*latch)->page_ << std::endl;
				// std::cout << (*latch)->page_->ToString();
				// std::cout << key->ToString() << std::endl;
				std::cout << "key existed ;)\n";
				assert(0);
			}
		}
	}
	return false;
}

Status BTree::Put(KeySlice *key)
{
	uint8_t depth = 0;
	page_id stack[8];

	auto latch = DescendToLeaf(key, stack, &depth);

	latch->Upgrade();

	if (latch->page_->Level()) {
		// std::cout << latch->page_->ToString();
		assert(latch->page_->Type() == BTreePage::ROOT);
		page_id page_no = latch->page_->Descend(key);
		assert(page_no);
		latch->Unlock();
		latch = latch_manager_->GetLatch(page_no);
		latch->Lock();
		// std::cout << page_no << std::endl;
		// std::cout << latch->page_->ToString();
		stack[depth++] = 0;
		assert(latch->page_->Level() == 0);
	}

	// assert(latch->page_->Level() == 0);

	// std::cout << key->ToString() << std::endl;
	bool split = Insert(&latch, key);

	BTreePage *left = latch->page_;
	for (; left->NeedSplit() || split; ) {
		if (left->Type() != BTreePage::ROOT) {
			if (!split) {
				auto right = BTreePage::NewPage(left->Type(), left->KeyLen(), left->Level(),
          left->Degree());
				left->Split(right, key);
			}
			Latch *pre = latch;
			page_id page_no = stack[--depth];
			latch = latch_manager_->GetLatch(page_no);
			latch->Lock();
			split = Insert(&latch, key);
			pre->Unlock();
			left = latch->page_;
		} else {
			SplitRoot(latch);
			break;
		}
	}
	latch->Unlock();
	++inserted_;
	return Success;
}

Status BTree::SplitRoot(Latch *latch)
{
	auto root = latch->page_;
	auto level = root->Level();
	BTreePage *left = BTreePage::NewPage(level ? BTreePage::BRANCH : BTreePage::LEAF,
		root->KeyLen(), level, degree_);
	BTreePage *right = BTreePage::NewPage(level ? BTreePage::BRANCH : BTreePage::LEAF,
	 	root->KeyLen(), level, degree_);

	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *slice = (KeySlice *)buf;

	root->Split(right, slice);

	page_id page_no = left->PageNo();
	left->Copy(root);

	root->Reset(0, BTreePage::ROOT, root->KeyLen(), root->Level()+1, root->Degree());
	root->AssignFirst(page_no);

	left->AssignPageNo(page_no);
	left->AssignType(right->Type());
	left->AssignLevel(right->Level());

	page_no = 0;
	char tmp[BTreePage::PageByte + key_len_] = {0};
	KeySlice *limit = (KeySlice *)tmp;
	memset(limit->Data(), 0xFF, key_len_);
	root->Insert(limit, page_no);
	root->Insert(slice, page_no);

	// std::cout << root->ToString();
	// std::cout << left->ToString();
	// std::cout << right->ToString();
	return Success;
}

Status BTree::Get(KeySlice *key) const
{
	uint8_t depth = 0;
	page_id stack[8];

	auto latch = DescendToLeaf(key, stack, &depth);
	// std::cout << latch->page_->ToString() << std::endl;
	bool flag = latch->page_->Search(key);
	latch->UnlockShared();
	return flag ? Success : Fail;
}

BTreePage* BTree::First(page_id *page_no, int level) const
{
	BTreePage *root = BTreePage::GetPage(0);
	if (level > root->Level())
		return nullptr;
	if (level == -1)
		level = root->Level();

	BTreePage *page = root;
	for (; page->Level() != level;)
		page = BTreePage::GetPage(page->First());

	if (page_no)
		*page_no = page->PageNo();
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

		assert(page->Level() == level);
	}
}

bool BTree::KeyCheck(std::ifstream &in, int total) const
{
	std::string val;
	char buf[BTreePage::PageByte + key_len_] = {0};
	KeySlice *key = (KeySlice *)buf;

	// std::cout << latch_manager_->ToString() << std::endl;

	in.seekg(0);
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
