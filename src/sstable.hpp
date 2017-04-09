/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 14:45:42
**/

#ifndef _SSTABLE_HPP_
#define _SSTABLE_HPP_

#include "utility.hpp"

namespace Mushroom {

class SSTable
{
	public:
		SSTable(const BLinkTree *b_link_tree);

		void Compact();

	private:
		const BLinkTree *b_link_tree_;
		uint32_t         total_;
		Block           *head_;
		KeySlice        *smallest_;
		KeySlice        *largest_;
};

} // namespace Mushroom

#endif /* _SSTABLE_HPP_ */