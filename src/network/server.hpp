/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:39
**/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <unordered_set>

#include "function.hpp"
#include "socket.hpp"

namespace Mushroom {

class Channel;
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

		void OnConnect(const ConnectCallBack &connectcb);

		Server(const Server &) = delete;
		Server& operator=(const Server &) = delete;

	private:
		Socket   socket_;
		Channel *listen_;
		Poller  *poller_;
		bool     running_;

		std::unordered_set<Connection *> connections_;

		ConnectCallBack connectcb_;

		void Accept();
};

} // namespace Mushroom

#endif /* _SERVER_HPP_ */