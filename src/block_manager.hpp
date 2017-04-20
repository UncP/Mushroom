/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-11 19:31:48
**/

#ifndef _BLOCK_MANAGER_HPP_
#define _BLOCK_MANAGER_HPP_

#include "utility.hpp"

#include <vector>
#include <stack>

namespace Mushroom {

class BlockManager
{
	public:
		BlockManager();

		~BlockManager();

		Block* NewBlock();

		void FreeBlock(block_t block_no);

	private:
		std::stack<block_t>  free_;
		std::vector<Block *> blocks_;
};

} // namespace Mushroom

#endif /* _BLOCK_MANAGER_HPP_ */
