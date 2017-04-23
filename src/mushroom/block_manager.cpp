/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-11 19:32:50
**/

#include "block_manager.hpp"
#include "block.hpp"

namespace Mushroom {

BlockManager::BlockManager() { }

BlockManager::~BlockManager()
{
	printf("\033[36mBlockManager Destructor\033[0m\ntotal: %lu  free: %lu\n",
		blocks_.size(), free_.size());
	for (uint32_t i = 0; i != blocks_.size(); ++i)
		delete blocks_[i];
}

Block* BlockManager::NewBlock()
{
	Block *block;
	if (!free_.empty()) {
		block_t block_no = free_.top();
		free_.pop();
		assert(block_no < blocks_.size());
		block = blocks_[block_no];
		block->Reset(block_no);
	} else {
		block = new Block(blocks_.size());
		blocks_.push_back(block);
	}
	return block;
}

void BlockManager::FreeBlock(block_t block_no)
{
	assert(block_no < blocks_.size());
	free_.push(block_no);
}

} // namespace Mushroom
