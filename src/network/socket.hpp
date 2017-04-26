/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:23:22
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

		bool Bind();

		bool Listen();

		int Accept();

		bool SetOption(int value, bool flag);

		bool GetOption(int value, int *ret);

		bool SetNonBlock(bool flag);

		bool GetPeerName(EndPoint *endpoint);

		bool GetSockName(EndPoint *endpoint);

		bool AddFlag(int flag);

	private:
		int      fd_;
};

} // namespace Mushroom

#endif /* _SOCKET_HPP_ */