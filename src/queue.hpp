/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-17 14:12:46
**/

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include <cassert>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <algorithm>

#include "task.hpp"
#include "slice.hpp"
#include "btree.hpp"

namespace Mushroom {

class Queue
{
	public:
		Queue(int capacity, uint8_t key_len);

		void Push(bool (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key);

		void Pull();

		void Clear();

		~Queue();

	private:
		bool                    clear_;
		int                     capacity_;
		std::vector<Task *>     queue_;
		std::vector<int>        avail_;
		std::vector<int>        work_;
		int                     front_;
		int                     avail_back_;
		int                     work_back_;
		std::mutex              mutex_;
		std::condition_variable ready_;
		std::condition_variable empty_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */