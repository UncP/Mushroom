/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:01
**/

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <functional>

#include "socket.hpp"
#include "endpoint.hpp"
#include "buffer.hpp"

namespace Mushroom {

typedef std::function<void()> ReadCallBack;
typedef std::function<void()> WriteCallBack;

class Connection
{
	public:
		Connection(const Socket &socket, uint32_t events);

		Connection(const EndPoint &server);

		Socket socket() const;

		uint32_t Events() const;

		bool Success() const;

		Buffer& GetInput();

		Buffer& GetOutput();

		void HandleRead();

		void HandleWrite();

		void OnRead(const ReadCallBack &readcb);

		void OnWrite(const WriteCallBack &writecb_);

		bool Close();

		void Send(const char *str);

		void Send(Buffer &buffer);

		uint32_t Send(const char *str, uint32_t len);

	private:
		Socket   socket_;
		uint32_t events_;
		bool     state_;
		Buffer   input_;
		Buffer   output_;

		ReadCallBack  readcb_;
		WriteCallBack writecb_;
};

} // namespace Mushroom

#endif /* _CONNECTION_HPP_ */