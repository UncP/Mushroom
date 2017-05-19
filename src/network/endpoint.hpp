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

class EndPoint
{
	public:
		static const uint32_t MaxLen = 16;

		EndPoint(uint16_t port, uint32_t address);

		EndPoint(uint16_t port, const char *str);

		~EndPoint();

		uint16_t Port() const;

		uint32_t Address() const;

		std::string ToString() const;

	private:
		uint16_t port_;
		uint32_t address_;
};

} // namespace Mushroom

#endif /* _ENDPOINT_HPP_ */