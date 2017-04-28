/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:39
**/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

namespace Mushroom {

class Connection;
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

	private:
		Connection *connection_;
		Poller     *poller_;
		bool        running_;
};

} // namespace Mushroom

#endif /* _SERVER_HPP_ */