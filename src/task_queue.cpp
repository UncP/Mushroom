/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 14:21:54
**/

#include "task_queue.hpp"

namespace Mushroom {

bool TaskQueue::Empty() const
{
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
}

void TaskQueue::Push(const Task &task)
{
	std::lock_guard<std::mutex> lock(mutex_);
	queue_.push(std::move(task));
	condition_.notify_one();
}

Task TaskQueue::Pop()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]{ return !queue_.empty(); });
	Task task(std::move(queue_.front()));
	queue_.pop();
	return std::move(task);
}

void TaskQueue::Clear()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]{ return queue_.empty(); });
}

} // namespace Mushroom
