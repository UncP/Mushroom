/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 23:25:32
**/

#ifndef _ENDPOINT_HPP_
#define _ENDPOINT_HPP_

#include <string>

namespace Mushroom {

const uint16_t ServerPort = 8000;

class EndPoint
{
	public:
		static const uint32_t MaxLen = 16;

		EndPoint();

		EndPoint(uint32_t address);

		EndPoint(const char *str);

		~EndPoint();

		uint32_t Address() const;

		std::string ToString() const;

	private:
		uint32_t address_;
};

} // namespace Mushroom

#endif /* _ENDPOINT_HPP_ */