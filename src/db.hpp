/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:32:20
**/

#ifndef _MUSHROOM_DB_HPP_
#define _MUSHROOM_DB_HPP_

#include <string>
#include <vector>

#include "latch_manager.hpp"
#include "btree.hpp"
#include "thread_pool.hpp"

namespace Mushroom {

class MushroomDB
{
	public:
		MushroomDB(const char *name, const int key_len);

		Status Put(KeySlice *key);

		Status Get(KeySlice *key);

		Status IndexSingle(const char *file, const int total = 1);

		Status IndexMultiple(const std::vector<std::string> &files, const int total = 1);

		Status FindSingle(const char *file, const int total = 1);

		Status FindMultiple(const std::vector<std::string> &files, const int total = 1);

		const BTree* Btree() const { return btree_; }

		Status Close();

		~MushroomDB() {
			delete btree_;
			delete pool_;
			delete latch_manager_;
		}

	private:
		BTree        *btree_;

		ThreadPool   *pool_;

		LatchManager *latch_manager_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_DB_HPP_ */