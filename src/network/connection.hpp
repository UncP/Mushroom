/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:01
**/

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "function.hpp"
#include "socket.hpp"
#include "endpoint.hpp"
#include "buffer.hpp"

namespace Mushroom {

class Channel;
class Poller;

class Connection
{
	public:
		Connection(const Socket &socket, uint32_t events, Poller *poller);

		Connection(const EndPoint &server);

		~Connection();

		bool Success() const;

		Buffer& GetInput();

		Buffer& GetOutput();

		void HandleRead();

		void HandleWrite();

		void OnRead(const ReadCallBack &readcb);

		void OnWrite(const WriteCallBack &writecb_);

		void OnSend(const SendCallBack &sendcb);

		bool Close();

		void Send(const char *str);

		void Send(Buffer &buffer);

		uint32_t Send(const char *str, uint32_t len);

	private:
		Socket   socket_;
		bool     connected_;
		Channel *channel_;
		Buffer   input_;
		Buffer   output_;

		ReadCallBack  readcb_;
		WriteCallBack writecb_;
		SendCallBack  sendcb_;
};

} // namespace Mushroom

#endif /* _CONNECTION_HPP_ */