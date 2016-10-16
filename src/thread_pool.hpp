/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "status.hpp"
#include "slice.hpp"
#include "btree.hpp"

namespace Mushroom {

class Job
{
	public:
		Job(Status (*fun)(const KeySlice *), BTree *btree, const KeySlice *key)
		:fun_(fun), btree_(btree), key_(key) { }

		void Execute() {
			btree_->fun_(key_);
		}

		~Job() { delete key_; }

	private:
		Status        (*fun_)(const KeySlice *);
		BTree          *btree_;
		const KeySlice *key_;
};

class Thread
{
	public:
		void Run();

		void Join() { thread_.join(); }

	private:
		std::thread             thread_;
		std::condition_variable condition_;
};

class ThreadPool
{
	public:
		ThreadPool(int thread_num = 4):thread_num_(thread_num) { }

		bool Init();

	private:
		int                 thread_num_;
		std::vector<Thread> threads_;
		std::queue<Job *>   queue_;
		std::mutex          mutex_;
		bool                working_;
};

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */