/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-14 21:19:07
**/

#ifndef _TIME_HPP_
#define _TIME_HPP_

#include <sys/time.h>

namespace Mushroom {

class Time
{
	public:
		static int64_t Now() {
			struct timeval tv;
			gettimeofday(&tv, 0);
			return (int64_t(tv.tv_sec) * 1000000 + tv.tv_usec) / 1000;
		}
};

} // namespace Mushroom

#endif /* _TIME_HPP_ */