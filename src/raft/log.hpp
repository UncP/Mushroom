/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-07 22:19:41
**/

#ifndef _RAFT_LOG_HPP_
#define _RAFT_LOG_HPP_

#include <cstdint>

namespace Mushroom {

class RaftServer;

class Log
{
	friend class RaftServer;
	public:
		Log();

		Log(uint32_t term, uint32_t number);

		uint32_t Term();

		uint32_t Number();

	private:
		uint32_t term_;
		uint32_t number_;
};

} // namespace Mushroom

#endif /* _RAFT_LOG_HPP_ */