/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:47:45
**/

#include <unistd.h>

#include "eventbase.hpp"
#include "../log/log.hpp"
#include "poller.hpp"
#include "channel.hpp"
#include "socket.hpp"

namespace Mushroom {

EventBase::EventBase():running_(true), poller_(new Poller())
{
	int r = pipe(wake_up_);
	FatalIf(r, "pipe failed, %s :(", strerror(errno));
	FatalIf(!Socket(wake_up_[0]).SetNonBlock(), "wake up fd set non-block failed :(");
	Channel *ch = new Channel(wake_up_[0], poller_, 0, 0);
	ch->OnRead([ch]() {
		char buf[16];
		ssize_t r = read(ch->fd(), buf, sizeof(buf));
		if (r > 0) {
			Info("wake up");
			delete ch;
		} else if (r == 0) {
		} else if (errno == EINTR) {
		} else {
			Fatal("pipe read error, %s :(", strerror(errno));
		}
	});
}

EventBase::~EventBase()
{
	close(wake_up_[1]);
	delete poller_;
}

Poller* EventBase::GetPoller()
{
	return poller_;
}

void EventBase::Loop()
{
	while (running_)
		poller_->LoopOnce(1000);
	poller_->LoopOnce(0);
}

void EventBase::Exit()
{
	if (running_) {
		running_ = false;
		WakeUp();
	}
}

void EventBase::WakeUp()
{
	ssize_t r = write(wake_up_[1], "", 1);
	FatalIf(r <= 0, "wake up failed, %s :(", strerror(errno));
}

} // namespace Mushroom
