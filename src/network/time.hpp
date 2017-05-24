/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-14 21:19:07
**/

#ifndef _TIME_HPP_
#define _TIME_HPP_

#include <sys/time.h>

namespace Mushroom {

class Time
{
	public:
		static int64_t Now() {
			return NowMicro() / 1000;
		}

		static int64_t NowMicro() {
			struct timeval tv;
			gettimeofday(&tv, 0);
			return (int64_t(tv.tv_sec) * 1000000 + tv.tv_usec);
		}
};

} // namespace Mushroom

#endif /* _TIME_HPP_ */