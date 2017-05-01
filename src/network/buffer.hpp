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

		uint32_t size() const;

		bool empty() const;

		char* data() const;

		char* begin() const;

		char* end() const;

		uint32_t space() const;

		void Append(const char *data, uint32_t len);

		void Consume(uint32_t len);

		void Expand(uint32_t len);

		void Clear();

	private:
		char     *data_;
		uint32_t  size_;
		uint32_t  beg_;
		uint32_t  end_;
};

} // namespace Mushroom

#endif /* _BUFFER_HPP_ */