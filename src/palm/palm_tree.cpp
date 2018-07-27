/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-27 17:23:00
**/

#include "palm_tree.hpp"
#include "../blink/page.hpp"
#include "../blink/pool_manager.hpp"
#include "barrier.hpp"

namespace Mushroom {

PalmTree::PalmTree(uint32_t key_len, uint32_t threads)
:pool_manager_(new PoolManager()), barrier_(new Barrier(threads)), root_(0), key_len_(key_len)
{
	degree_ = Page::CalculateDegree(key_len_);
	Set set;
	set.page_ = pool_manager_->NewPage(Page::ROOT, key_len_, 0, degree_);
	set.page_->InsertInfiniteKey();
}

PalmTree::~PalmTree()
{
	delete barrier_;
	delete pool_manager_;
}

void PalmTree::Free()
{
	pool_manager_->Free();
}

bool PalmTree::Put(KeySlice *key)
{

}

bool PalmTree::Get(KeySlice *key)
{

}

} // Mushroom
