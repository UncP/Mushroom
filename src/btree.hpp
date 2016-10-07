/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-07 20:10:30
**/

#ifndef _BTREE_HPP_
#define _BTREE_HPP_

#include <stdint.h>

namespace Mushroom {

class BTreePage
{
	public:
		enum { ROOT = 0, BRANCH, LEAF };

	private:
		uint8_t  type_;
		uint32_t page_no_;
		uint8_t  key_num_;
		uint8_t  child_num_;
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */