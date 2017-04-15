/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 16:56:06
**/

#ifndef NOLSM

#include "slice.hpp"
#include "sstable.hpp"
#include "b_link_tree.hpp"
#include "block.hpp"
#include "block_manager.hpp"

namespace Mushroom {

uint32_t SSTable::FileSize(uint32_t level)
{
	// level = (level + 1) * 3;
	assert(level < 32);
	return uint32_t(1) << level;
}

SSTable::SSTable(const BLinkTree *b_link_tree, BlockManager *block_manager):pin_(false)
{
	BLinkTree::Iterator iter(b_link_tree);
	info_.key_len_ = b_link_tree->KeyLength();
	blocks_.push_back(block_manager->NewBlock());
	Block *curr = blocks_[info_.block_num_++];
	for (;;) {
		if (!curr->Append((char *)iter.key_->key_, info_.key_len_)) {
			info_.AppendKeyRange(curr);
			blocks_.push_back(block_manager->NewBlock());
			curr = blocks_[info_.block_num_++];
		} else if (!iter.Next()) {
			break;
		}
	}
	info_.AppendKeyRange(curr);
}

SSTable::SSTable(uint32_t level, uint32_t key_len, table_t table_no)
:table_no_(table_no), level_(level), pin_(true)
{
	info_.key_len_ = key_len;
}

bool SSTable::Append(const Key &key, BlockManager *block_manager)
{
	if (!info_.block_num_) {
		blocks_.push_back(block_manager->NewBlock());
		++info_.block_num_;
	}
	Block *curr = blocks_[info_.block_num_-1];
	if (!curr->Append(key.data_, key.size_)) {
		info_.AppendKeyRange(curr);
		if (ReachThreshold())
			return false;
		blocks_.push_back(block_manager->NewBlock());
		curr = blocks_[info_.block_num_++];
		curr->Append(key.data_, key.size_);
	}
	return true;
}

bool SSTable::ReachThreshold()
{
	return (info_.block_num_ * Block::BlockSize) >= (FileSize(level_) * 1048576);
}

void SSTable::BlockInfo::AppendKeyRange(const Block *block)
{
	Block::Iterator iter(block, key_len_);
	iter.First();
	smallest_.push_back(std::string(iter.key_.data_, key_len_));
	iter.Last();
	largest_.push_back(std::string(iter.key_.data_, key_len_));
}

void SSTable::FormKeySlice(KeySlice *slice) const
{
	assert(info_.block_num_);
	const std::string &smallest = info_.smallest_[0];
	const std::string &largest  = info_.largest_[info_.block_num_-1];
	CopyPrefix(slice, smallest.c_str(), info_.key_len_);
	CopySuffix(slice, largest.c_str(), info_.key_len_, info_.key_len_);
	slice->tptr_ = table_no_;
}

void SSTable::Reset(uint32_t level, uint32_t key_len, table_t table_no)
{
	table_no_ = table_no;
	level_ = level;
	pin_ = true;
	blocks_.clear();
	info_.key_len_ = key_len;
	info_.block_num_ = 0;
	info_.smallest_.clear();
	info_.largest_.clear();
}

} // namespace Mushroom

#endif /* NOLSM */
