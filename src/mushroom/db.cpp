/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-10 15:34:43
**/

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
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
	PoolManager::SetManagerInfo(page_size, pool_size, hash_bits, seg_bits);

	LogManager::SetManagerInfo(log_page);

	if (access(name, F_OK))
		assert(mkdir(name, S_IRUSR | S_IWUSR) >= 0);

	tree_ = new BLinkTree(name, key_len);

	log_manager_ = new LogManager(name);

	// Page *redo;
	// if ((redo = log_manager_->NeedRecover())) {
	// 	for (uint32_t i = 0; i < LogManager::LogPage; ++i)
	// 		tree_->BatchPut(redo + i * Page::PageSize);
	// }
}

MushroomDB::~MushroomDB()
{
	delete tree_;
	delete log_manager_;
}

bool MushroomDB::Put(KeySlice *key)
{
	return tree_->Put(key);
}

bool MushroomDB::BatchPut(Page *page)
{
	if (log_manager_->NeedFlush())
		tree_->FlushDirtyPages();
	log_manager_->Logging(page);
	return tree_->BatchPut(page);
}

bool MushroomDB::Get(KeySlice *key)
{
	return tree_->Get(key);
}

void MushroomDB::Close()
{
	tree_->Free();
}

} // namespace Mushroom
