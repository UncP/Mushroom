/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 16:56:06
**/

#ifdef LSM

#include "slice.hpp"
#include "sstable.hpp"
#include "b_link_tree.hpp"
#include "block.hpp"

namespace Mushroom {

SSTable::Info::Info():block_size_(0) { }

void SSTable::Info::AppendKeyRange(const Block *block)
{
	Block::Iterator iter(block);
	iter.First();
	smallest_.push_back(std::string(iter.key_, key_len_));
	iter.Last();
	largest_.push_back(std::string(iter.key_, key_len_));
}

SSTable::SSTable(const BLinkTree *b_link_tree)
{
	BLinkTree::Iterator iter(b_link_tree);
	info_.key_len_ = b_link_tree->KeyLength();
	blocks_.push_back(new Block(info_.key_len_));
	Block *curr = blocks_[info_.block_size_++];
	for (;;) {
		if (!curr->Append((char *)iter.key_->key_)) {
			info_.AppendKeyRange(curr);
			blocks_.push_back(new Block(info_.key_len_));
			curr = blocks_[info_.block_size_++];
		} else if (!iter.Next()) {
			break;
		}
	}
	info_.AppendKeyRange(curr);
}

SSTable::~SSTable()
{
	for (size_t i = 0; i != blocks_.size(); ++i)
		delete blocks_[i];
}

} // namespace Mushroom

#endif
