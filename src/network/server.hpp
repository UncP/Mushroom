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
class EventBase;

class Server : private NoCopy
{
	public:
		Server(EventBase *event_base);

		virtual ~Server();

		bool Start();

		void OnConnect(const ConnectCallBack &connectcb);

	protected:
		EventBase *event_base_;
		Socket     socket_;
		Channel   *listen_;

		std::vector<Connection *> connections_;
		ConnectCallBack           connectcb_;

	private:
		void HandleAccept();
};

} // namespace Mushroom

#endif /* _SERVER_HPP_ */