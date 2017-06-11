/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-10 15:32:20
**/

#ifndef _MUSHROOM_DB_HPP_
#define _MUSHROOM_DB_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class KeySlice;
class BLinkTree;

class MushroomDB : private NoCopy
{
	public:
		MushroomDB(const char *name, uint32_t key_len, uint32_t page_size, uint32_t pool_size,
			uint32_t hash_bits, uint32_t seg_bits, uint32_t log_page);

		bool Put(KeySlice *key);

		bool Get(KeySlice *key);

		void Close();

		~MushroomDB();

	private:
		BLinkTree    *tree_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_DB_HPP_ */