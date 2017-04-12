/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-11 19:32:50
**/

#ifndef NOLSM

#include "block_manager.hpp"
#include "block.hpp"

namespace Mushroom {

BlockManager::BlockManager():pinned_(0), tail_(0), unpin_(0) { }

BlockManager::~BlockManager()
{
	for (; pinned_;) {
		Block *next = pinned_->next_;
		delete pinned_;
		pinned_ = next;
	}
	for (; unpin_;) {
		Block *next = unpin_->next_;
		delete unpin_;
		unpin_ = next;
	}
}

Block* BlockManager::NewBlock()
{
	Block *block;
	if (unpin_) {
		block  = unpin_;
		block->pin_ = true;
		unpin_ = unpin_->next_;
		return block;
	}
	block = new Block();
	if (!tail_) {
		pinned_ = block;
		tail_ = block;
	} else {
		tail_->next_ = block;
		tail_ = tail_->next_;
	}
	return block;
}

} // namespace Mushroom

#endif
