/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:34:43
**/

#include "db.hpp"
#include "b_link_tree.hpp"
#include "pool_manager.hpp"

#ifndef NOLATCH
#include "latch_manager.hpp"
#endif

namespace Mushroom {

MushroomDB::MushroomDB(const int key_len, uint32_t page_size,
	uint32_t pool_size, uint32_t hash_bits, uint32_t seg_bits)
{
	#ifndef NOLATCH
	LatchManager *latch_manager = new LatchManager();
	#endif

	PoolManager *page_manager = new PoolManager(page_size, pool_size, hash_bits, seg_bits);

	#ifndef NOLATCH
	b_link_tree_ = new BLinkTree(key_len, latch_manager, page_manager);
	#else
	b_link_tree_ = new BLinkTree(key_len, page_manager);
	#endif

	b_link_tree_->Initialize();
}

MushroomDB::~MushroomDB()
{
	delete b_link_tree_;
}

bool MushroomDB::Put(KeySlice *key)
{
	return b_link_tree_->Put(key);
}

bool MushroomDB::Get(KeySlice *key)
{
	return b_link_tree_->Get(key);
}

bool MushroomDB::FindSingle(int fd, int total)
{
	return b_link_tree_->Check(fd, total);
}

bool MushroomDB::Close()
{
	b_link_tree_->Free();
	return true;
}

} // namespace Mushroom
