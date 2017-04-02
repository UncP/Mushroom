/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:34:43
**/

#include "db.hpp"
#include "b_link_tree.hpp"
#include "pool_manager.hpp"
#include "latch_manager.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, const int key_len, uint32_t page_size,
	uint32_t pool_size, uint8_t hash_bits, uint8_t seg_bits)
{
	PoolManager::SetPoolManagerInfo(page_size, pool_size, hash_bits, seg_bits);

	LatchManager *latch_manager = new LatchManager();

	PoolManager *page_manager = new PoolManager();

	blinktree_ = new BLinkTree(key_len, latch_manager, page_manager);

	blinktree_->Initialize();
}

MushroomDB::~MushroomDB()
{
	delete blinktree_;
}

bool MushroomDB::Put(KeySlice *key)
{
	return blinktree_->Put(key);
}

bool MushroomDB::Get(KeySlice *key)
{
	return blinktree_->Get(key);
}

bool MushroomDB::FindSingle(int fd, int total)
{
	return blinktree_->Check(fd, total);
}

bool MushroomDB::Close()
{
	blinktree_->Free();
	return true;
}

} // namespace Mushroom
