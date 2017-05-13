/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:45:21
**/

#ifndef _EVENT_BASE_HPP_
#define _EVENT_BASE_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class Poller;

class EventBase : private NoCopy
{
	public:
		EventBase();

		~EventBase();

		void Loop();

		void Exit();

		Poller* GetPoller();

	private:
		void WakeUp();

		bool     running_;
		int      wake_up_[2];
		Poller  *poller_;
};

} // namespace Mushroom

#endif /* _EVENT_BASE_HPP_ */