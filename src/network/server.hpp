/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:39
**/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <vector>

#include "../utility/utility.hpp"
#include "function.hpp"
#include "socket.hpp"

namespace Mushroom {

class Channel;
class Poller;

class Server : private NoCopy
{
	public:
		Server();

		virtual ~Server();

		bool Start();

		bool Close();

		void Run();

		void OnConnect(const ConnectCallBack &connectcb);

	protected:
		Socket   socket_;
		Channel *listen_;
		Poller  *poller_;

		std::vector<Connection *> connections_;

	private:
		bool                      running_;
		ConnectCallBack           connectcb_;

		void HandleAccept();
};

} // namespace Mushroom

#endif /* _SERVER_HPP_ */