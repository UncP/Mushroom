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

		Status Close();

	private:
		std::string file_;

		BTree       btree_;

};

} // namespace Mushroom