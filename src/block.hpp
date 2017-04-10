/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:32
**/

#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

#include "utility.hpp"

namespace Mushroom {

class Block
{
	public:
		static uint32_t BlockSize;

		Block();

		~Block();

		inline bool Append(const char *data, uint32_t len) {
			if (off_ + len < BlockSize) {
				memcpy(mem_, data, len);
				return true;
			}
			return false;
		}

	private:
		char     *mem_;
		KeySlice *smallest_;
		KeySlice *largest_;
		uint32_t  off_;
};

} // namespace Mushroom

#endif /* _BLOCK_HPP_ */