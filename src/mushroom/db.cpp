/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-10 15:34:43
**/

#include <unistd.h>
#include <cassert>

#include "db.hpp"
#include "pool_manager.hpp"
#include "b_link_tree.hpp"
#include "pool_manager.hpp"
#include "log_manager.hpp"
#include "page.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, uint32_t key_len, uint32_t page_size,
	uint32_t pool_size, uint32_t hash_bits, uint32_t seg_bits, uint32_t log_page)
{
	assert(name);

	PoolManager::SetManagerInfo(page_size, pool_size, hash_bits, seg_bits);

	LogManager::SetManagerInfo(log_page);

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

void MushroomDB::Close()
{
	tree_->Free();
}

bool MushroomDB::operator==(const MushroomDB &that) const
{
	return *tree_ == *that.tree_;
}

} // namespace Mushroom
