/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 15:32:20
**/

#include <string>

#include "btree.hpp"

namespace Mushroom {

class MushroomDB
{
	public:
		MushroomDB() { }

		Status Open(const char *file, const int key_len);

		Status Put(const KeySlice *key) {
			return btree_->Put(key);
		}

		const BTree* Btree() const { return btree_ ; }

		Status Close();

		~MushroomDB() { if (btree_) delete btree_; btree_ = nullptr; }

	private:
		std::string file_;

		BTree      *btree_;
};

} // namespace Mushroom