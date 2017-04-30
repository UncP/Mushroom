/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 17:07:38
**/

#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include "function.hpp"

namespace Mushroom {

class Poller;

class Channel
{
	public:
		friend class Connection;

		Channel(int fd, uint32_t events, Poller *poller);

		~Channel();

		int fd() const;

		uint32_t events() const;

		void OnRead(const ReadCallBack &readcb);

		void OnWrite(const WriteCallBack &writecb);

		void HandleRead();

		void HandleWrite();

	private:
		int      fd_;
		uint32_t events_;
		Poller  *poller_;

		ReadCallBack  readcb_;
		WriteCallBack writecb_;
};

} // namespace Mushroom

#endif /* _CHANNEL_HPP_ */