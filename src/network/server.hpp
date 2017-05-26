/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-23 10:50:39
**/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <vector>

#include "../include/utility.hpp"
#include "callback.hpp"
#include "socket.hpp"

namespace Mushroom {

class Channel;
class EventBase;

class Server : private NoCopy
{
	public:
		Server(EventBase *event_base, uint16_t port);

		virtual ~Server();

		void Start();

		void Close();

		void OnConnect(const ConnectCallBack &connectcb);

		std::vector<Connection *>& Connections();

		uint16_t Port() const;

	protected:
		uint16_t   port_;
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