/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 14:12:46
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

		void Push(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key);

		Task* Pull();

		void Clear();

		~Queue();

	private:

		bool Empty();

		bool Empty(int);

		bool Full();

		bool                    clear_;
		std::mutex              mutex_;
		std::condition_variable condition_;
		std::vector<Task *>     queue_;
		int                     capacity_;
		int                     front_;
		int                     back_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */