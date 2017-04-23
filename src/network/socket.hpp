/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:23:22
**/

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

namespace Mushroom {

class Socket
{
	public:
		void Close();

	private:
		int fd_;
};

} // namespace Mushroom

#endif /* _SOCKET_HPP_ */