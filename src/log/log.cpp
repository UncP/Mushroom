/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Githup:    https://www.githup.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 09:37:28
**/

#include <cstdarg>
#include <cstdio>

#include "log.hpp"

namespace Mushroom {

Logv::Logv() { }

Logv& Logv::GetLogger() {
	static Logv log;
	return log;
}

void Logv::logv(const char *fmt ...)
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
