/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Githup:    https://www.githup.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 09:37:28
**/

#include <cstdio>
#include <cstdarg>

#include "log.hpp"

namespace Mushroom {

Log::Log() { }

Log& Log::GetLogger()
{
	static Log lg;
	return lg;
}

void Log::logv(const char *fmt ...)
{
	char buf[1024];
	char *p = buf;
	char *e = buf + sizeof(buf);
	va_list args;
	va_start(args, fmt);
	p += vsnprintf(p, e - p, fmt, args);
	va_end(args);
	*p++ = '\n';
	*p++ = '\0';
	printf("%s", buf);
}

} // namespace Mushroom
