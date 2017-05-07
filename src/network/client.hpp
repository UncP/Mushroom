/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:42:08
**/

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

namespace Mushroom {

class EventBase;

class Client
{
	public:
		Client(EventBase *base);

		~Client();

	private:
		EventBase *base_;
};

} // namespace Mushroom

#endif /* _CLIENT_HPP_ */