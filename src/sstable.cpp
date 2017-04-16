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

SSTable::SSTable():pin_(false) { }

SSTable::SSTable(table_t table_no, uint32_t level, uint32_t key_len)
:table_no_(table_no), level_(level), key_len_(key_len) { }

void SSTable::Generate(const BLinkTree *b_link_tree, BlockManager *block_manager)
{
	BLinkTree::Iterator iter(b_link_tree);
	key_len_ = b_link_tree->KeyLength();
	blocks_.push_back(block_manager->NewBlock());
	Block *curr = blocks_[info_.block_num_++];
	for (;;) {
		if (!curr->Append((char *)iter.key_->key_, key_len_)) {
			info_.AppendKeyRange(curr, key_len_);
			blocks_.push_back(block_manager->NewBlock());
			curr = blocks_[info_.block_num_++];
		} else if (!iter.Next()) {
			break;
		}
	}
	info_.AppendKeyRange(curr, key_len_);
}

bool SSTable::Append(const Key &key, BlockManager *block_manager)
{
	if (!info_.block_num_) {
		blocks_.push_back(block_manager->NewBlock());
		++info_.block_num_;
	}
	Block *curr = blocks_[info_.block_num_-1];
	if (!curr->Append(key.data_, key.size_)) {
		info_.AppendKeyRange(curr, key_len_);
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

void SSTable::BlockInfo::AppendKeyRange(const Block *block, uint32_t key_len)
{
	Block::Iterator iter(block, key_len);
	iter.First();
	smallest_.push_back(std::string(iter.key_.data_, key_len));
	iter.Last();
	largest_.push_back(std::string(iter.key_.data_, key_len));
}

void SSTable::FormKeySlice(KeySlice *slice) const
{
	assert(info_.block_num_);
	const std::string &smallest = info_.smallest_[0];
	const std::string &largest  = info_.largest_[info_.block_num_-1];
	CopyPrefix(slice, smallest.c_str(), key_len_);
	CopySuffix(slice, largest.c_str(), key_len_, key_len_);
	slice->tptr_ = table_no_;
}

void SSTable::Reset()
{
	table_no_ = 0;
	level_ = 0;
	key_len_ = 0;
	pin_ = false;
	blocks_.clear();
	info_.block_num_ = 0;
	info_.smallest_.clear();
	info_.largest_.clear();
}

void SSTable::Reset(table_t table_no, uint32_t level, uint32_t key_len)
{
	table_no_ = table_no;
	level_ = level;
	key_len_ = key_len;
	pin_ = true;
	blocks_.clear();
	info_.block_num_ = 0;
	info_.smallest_.clear();
	info_.largest_.clear();
}

} // namespace Mushroom

#endif /* NOLSM */
