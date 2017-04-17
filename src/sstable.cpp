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

uint32_t SSTable::MaxSizeInBytes(uint32_t level)
{
	assert(++level < 32);
	return (uint32_t(1) << level) * 1048576;
}

SSTable::SSTable(table_t table_no, uint32_t level):table_no_(table_no), level_(level) { }

void SSTable::Generate(const BLinkTree *b_link_tree, BlockManager *block_manager)
{
	uint32_t key_len = b_link_tree->KeyLength();
	BLinkTree::Iterator iter(b_link_tree);
	blocks_.push_back(block_manager->NewBlock());
	Block *curr = blocks_[block_num_++];
	for (;;) {
		if (!curr->Append((char *)iter.key_->key_, key_len)) {
			AppendKeyRange(curr, key_len);
			blocks_.push_back(block_manager->NewBlock());
			curr = blocks_[block_num_++];
		} else if (!iter.Next()) {
			break;
		}
	}
	AppendKeyRange(curr, key_len);
}

bool SSTable::Append(const Key &key, BlockManager *block_manager)
{
	if (!block_num_) {
		blocks_.push_back(block_manager->NewBlock());
		++block_num_;
	}
	Block *curr = blocks_[block_num_-1];
	if (!curr->Append(key.data_, key.size_)) {
		AppendKeyRange(curr, key.size_);
		if ((block_num_ * Block::BlockSize) >= SSTable::MaxSizeInBytes(level_))
			return false;
		blocks_.push_back(block_manager->NewBlock());
		curr = blocks_[block_num_++];
		curr->Append(key.data_, key.size_);
	}
	return true;
}

void SSTable::AppendKeyRange(const Block *block, uint32_t key_len)
{
	Block::Iterator iter(block, key_len);
	iter.First();
	smallest_.push_back(std::string(iter.key_.data_, key_len));
	iter.Last();
	largest_.push_back(std::string(iter.key_.data_, key_len));
}

void SSTable::Reset(table_t table_no, uint32_t level)
{
	table_no_ = table_no;
	level_ = level;
	block_num_ = 0;
	blocks_.clear();
	smallest_.clear();
	largest_.clear();
}

} // namespace Mushroom

#endif /* NOLSM */
