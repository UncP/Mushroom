/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-27 09:44:37
**/

#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include <cstdint>

namespace Mushroom {

class Buffer
{
	public:
		static const uint32_t BufferSize = 4096;

		Buffer();

		~Buffer();

	private:
		char    *data_;
		uint32_t size_;
		uint32_t beg_;
		uint32_t end_;
};

} // namespace Mushroom

#endif /* _BUFFER_HPP_ */