/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-19 10:24:14
**/

#ifndef _GUARD_HPP_
#define _GUARD_HPP_

#include "utility.hpp"
#include "mutex.hpp"

namespace Mushroom {

class Guard : private NoCopy
{
	public:
		Guard(Mutex &mutex):mutex_(mutex) { mutex_.Lock(); }

		~Guard() { mutex_.Unlock(); }

	private:
		Mutex &mutex_;
};

} // namespace Mushroom

#endif /* _GUARD_HPP_ */