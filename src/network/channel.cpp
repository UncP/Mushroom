/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 17:07:49
**/

#include "channel.hpp"
#include "poller.hpp"

namespace Mushroom {

Channel::Channel(int fd, Poller *poller, const ReadCallBack &readcb,
	const WriteCallBack &writecb)
:fd_(fd), poller_(poller), readcb_(readcb), writecb_(writecb)
{
	events_ = ReadEvent;
	poller_->AddChannel(this);
}

Channel::~Channel()
{
	poller_->RemoveChannel(this);
	fd_      = -1;
	poller_  = 0;
	readcb_  = 0;
	writecb_ = 0;
}

int Channel::fd() const
{
	return fd_;
}

uint32_t Channel::events() const
{
	return events_;
}

void Channel::OnRead(const ReadCallBack &readcb)
{
	readcb_ = readcb;
}

void Channel::OnWrite(const WriteCallBack &writecb)
{
	writecb_ = writecb;
}

bool Channel::CanRead() const
{
	return events_ & ReadEvent;
}

bool Channel::CanWrite() const
{
	return events_ & WriteEvent;
}

void Channel::EnableRead(bool flag)
{
	if (flag)
		events_ |= ReadEvent;
	else
		events_ &= ~ReadEvent;

	poller_->UpdateChannel(this);
}

void Channel::EnableWrite(bool flag)
{
	if (flag)
		events_ |= WriteEvent;
	else
		events_ &= ~WriteEvent;

	poller_->UpdateChannel(this);
}

void Channel::HandleRead()
{
	readcb_();
}

void Channel::HandleWrite()
{
	writecb_();
}

} // namespace Mushroom
