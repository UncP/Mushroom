/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-11 19:31:48
**/

#ifndef NOLSM

#ifndef _BLOCK_MANAGER_HPP_
#define _BLOCK_MANAGER_HPP_

#include "utility.hpp"

namespace Mushroom {

class BlockManager
{
	public:
		BlockManager();

		~BlockManager();

		Block* NewBlock();

	private:
		Block *pinned_;
		Block *tail_;
		Block *unpin_;
};

} // namespace Mushroom

#endif /* _BLOCK_MANAGER_HPP_ */

#endif