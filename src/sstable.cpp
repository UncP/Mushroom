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

SSTable::SSTable(table_t table_no):table_no_(table_no), pin_(true) { }

SSTable::SSTable(const BLinkTree *b_link_tree, BlockManager *block_manager,
	table_t table_no):table_no_(table_no), pin_(true)
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

} // namespace Mushroom

#endif
