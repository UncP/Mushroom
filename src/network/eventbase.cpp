/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:47:45
**/

#include "eventbase.hpp"
#include "poller.hpp"

namespace Mushroom {

EventBase::EventBase():running_(true), poller_(new Poller()) { }

EventBase::~EventBase()
{
	delete poller_;
}

Poller* EventBase::GetPoller()
{
	return poller_;
}

void EventBase::Loop()
{
	while (running_)
		poller_->LoopOnce();
}

void EventBase::Exit()
{
	running_ = false;
}

} // namespace Mushroom
