/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 09:18:05
**/

#include "marshaller.hpp"

namespace Mushroom {

Marshaller::Marshaller(Buffer &input, Buffer &output):input_(input), output_(output) { }

Marshaller::~Marshaller() { }

void Marshaller::Read(const void *str, uint32_t len)
{
	output_.Read((const char *)str, len);
}

void Marshaller::Write(void *str, uint32_t len)
{
	input_.Write((char *)str, len);
}

} // namespace Mushroom
