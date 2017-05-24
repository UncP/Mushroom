/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-10 15:34:43
**/

#include "db.hpp"
#include "pool_manager.hpp"
#include "b_link_tree.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(uint32_t key_len, uint32_t page_size,
	uint32_t pool_size, uint32_t hash_bits, uint32_t seg_bits)
{
	PoolManager::SetManagerInfo(page_size, pool_size, hash_bits, seg_bits);
	tree_ = new BLinkTree(key_len);
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
