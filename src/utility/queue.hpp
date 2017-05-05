/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-05 22:48:10
**/

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include "latch.hpp"

namespace Mushroom {

template<typename T>
class Queue
{
	public:

	private:
		bool               clear_;
		int                capacity_;
		T*                *queue_;
		int               *avail_;
		int               *work_;
		int                front_;
		int                avail_back_;
		int                work_back_;
		Mutex              mutex_;
		Cond               ready_;
		Cond               empty_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */