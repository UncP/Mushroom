/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 23:25:45
**/

#include <cassert>
#include <arpa/inet.h>

#include "endpoint.hpp"

namespace Mushroom {

EndPoint::EndPoint() { }

EndPoint::EndPoint(const char *str)
{
	assert(inet_pton(AF_INET, str, &address_) == 1);
}

EndPoint::~EndPoint() { }

uint32_t EndPoint::Address() const
{
	return address_;
}

std::string EndPoint::ToString() const
{
	char buf[MaxLen];
	assert(inet_ntop(AF_INET, &address_, buf, MaxLen));
	return std::string(buf);
}

} // namespace Mushroom
