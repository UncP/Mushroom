/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 17:07:38
**/

#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include "../include/utility.hpp"
#include "callback.hpp"

namespace Mushroom {

class Poller;

class Channel : private NoCopy
{
	public:
		friend class Connection;

		Channel(int fd, Poller *poller, const ReadCallBack &readcb, const WriteCallBack &writecb);

		~Channel();

		int fd() const;

		uint32_t events() const;

		bool CanRead() const;

		bool CanWrite() const;

		void OnRead(const ReadCallBack &readcb);

		void OnWrite(const WriteCallBack &writecb);

		void EnableRead(bool flag);

		void EnableWrite(bool flag);

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