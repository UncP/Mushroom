/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 14:45:42
**/

#ifndef _SKTABLE_HPP_
#define _SKTABLE_HPP_

#include "utility.hpp"

namespace Mushroom {

class SKTable
{
	public:
		SKTable();

		void Compact(const BLinkTree *b_link_tree);

	private:
};

} // namespace Mushroom

#endif /* _SKTABLE_HPP_ */