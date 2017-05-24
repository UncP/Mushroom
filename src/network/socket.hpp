/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-23 10:23:22
**/

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include "endpoint.hpp"

namespace Mushroom {

class Socket
{
	public:
		Socket();

		Socket(int fd);

		~Socket();

		int fd() const;

		bool Valid() const;

		bool Create();

		bool Close();

		bool Connect(const EndPoint &end_point);

		bool Bind(uint16_t port);

		bool Listen();

		int Accept();

		uint32_t Write(const char *data, uint32_t len);

		uint32_t Read(char *data, uint32_t len, bool *blocked);

		bool SetOption(int value, bool flag);

		bool GetOption(int value, int *ret);

		bool GetPeerName(EndPoint *endpoint);

		bool GetSockName(EndPoint *endpoint);

		bool AddFlag(int flag);

		bool SetNonBlock();

		bool SetResuseAddress();

	private:
		int      fd_;
};

} // namespace Mushroom

#endif /* _SOCKET_HPP_ */