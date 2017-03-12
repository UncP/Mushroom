/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 15:32:20
**/

#ifndef _MUSHROOM_DB_HPP_
#define _MUSHROOM_DB_HPP_

#include <string>

#include "btree.hpp"
#include "thread_pool.hpp"

namespace Mushroom {

class MushroomDB
{
	public:
		MushroomDB(const char *name, const int key_len);

		Status Put(KeySlice *key);

		Status Get(KeySlice *key);

		const BTree* Btree() const { return btree_ ; }

		Status Close();

		void ClearTask() { if (pool_) pool_->Clear(); }

		~MushroomDB() {
			delete btree_;
			delete pool_;
		}

	private:
		std::string name_;

		BTree      *btree_;

		ThreadPool *pool_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_DB_HPP_ */