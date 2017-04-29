/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:39
**/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "../log/log.hpp"
#include "connection.hpp"

namespace Mushroom {

typedef std::function<void(Connection *)> ServerReadCallBack;
typedef std::function<void(Connection *)> ServerWriteCallBack;

class Poller;

class Server
{
	public:
		Server();

		~Server();

		bool Start();

		void Stop();

		bool Close();

		void Run();

		void OnRead(const ServerReadCallBack &readcb);

		void OnWrite(const ServerWriteCallBack &writecb);

	private:
		Connection *connection_;
		Poller     *poller_;
		bool        running_;

		ServerReadCallBack  readcb_;
		ServerWriteCallBack writecb_;
};

} // namespace Mushroom

#endif /* _SERVER_HPP_ */