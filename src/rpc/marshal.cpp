/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 09:18:05
**/

#include "marshal.hpp"

namespace Mushroom {

Marshal::Marshal(Buffer &input, Buffer &output):input_(input), output_(output) { }

Marshal::~Marshal() { }

void Marshal::Read(const void *str, uint32_t len)
{
	output_.Expand((const char *)str, len);
}

void Marshal::Write(void *str, uint32_t len)
{
	input_.Consume((char *)str, len);
}

} // namespace Mushroom
