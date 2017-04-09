/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:32
**/

#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

namespace Mushroom {

class Block
{
	public:
		static uint32_t BlockSize;

		Block();

		~Block();

		inline void Append(const char *data, uint32_t len);

	private:
		char    *mem_;
		uint32_t off_;
		Block   *next_;
};

} // namespace Mushroom

#endif /* _BLOCK_HPP_ */