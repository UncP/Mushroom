/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include "raft.hpp"

namespace Mushroom {

Raft::Raft(uint32_t heartbeat_interval, uint32_t election_interval)
:state_(Follower), term_(0), vote_for_(-1), commit_(-1), applied_(-1),
heartbeat_interval_(heartbeat_interval), election_interval_(election_interval)
{ }

Raft::~Raft() { }

Raft::Run()
{

}

} // namespace Mushroom
