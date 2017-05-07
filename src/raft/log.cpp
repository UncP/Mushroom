/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-07 22:19:46
**/

#include "log.hpp"

namespace Mushroom {

Log::Log() { }

Log::Log(uint32_t term, uint32_t number):term_(term), number_(number) { }

uint32_t Log::Term() const
{
	return term_;
}

uint32_t Log::Number() const
{
	return number_;
}

} // namespace Mushroom
