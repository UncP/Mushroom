/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:34:43
**/

#include "db.hpp"
#ifndef NOLSM
#include "lsm_tree.hpp"
#else
#include "b_link_tree.hpp"
#endif
#include "pool_manager.hpp"

#ifndef NOLATCH
#include "latch_manager.hpp"
#endif

namespace Mushroom {

MushroomDB::MushroomDB(uint32_t key_len, uint32_t page_size,
	uint32_t pool_size, uint32_t hash_bits, uint32_t seg_bits)
{
	PoolManager::SetManagerInfo(page_size, pool_size, hash_bits, seg_bits);
	#ifndef NOLSM
	tree_ = new LSMTree(2, key_len);
	#else
	tree_ = new BLinkTree(key_len);
	#endif
}

MushroomDB::~MushroomDB()
{
	delete tree_;
}

bool MushroomDB::Put(KeySlice *key)
{
	return tree_->Put(key);
}

bool MushroomDB::Get(KeySlice *key)
{
	return tree_->Get(key);
}

bool MushroomDB::Close()
{
	tree_->Free();
	return true;
}

} // namespace Mushroom
