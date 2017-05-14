/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:45:21
**/

#ifndef _EVENT_BASE_HPP_
#define _EVENT_BASE_HPP_

#include <map>

#include "../include/utility.hpp"
#include "../include/atomic.hpp"
#include "../include/mutex.hpp"

namespace Mushroom {

class Poller;
template<typename T> class ThreadPool<T>;
template<typename T> class BoundedQueue<T>;

typedef std::pair<int64_t, uint32_t> TimerId;

class EventBase : private NoCopy
{
	public:
		EventBase(int thread_num = 1, int queue_size = 16);

		~EventBase();

		void Loop();

		void Exit();

		Poller* GetPoller();

		void RunNow(const Task &task);

		TimerId RunAfter(int64_t milli_sec, const Task &task);

		TimerId RunEvery(int64_t milli_sec, const Task &task);

		void Cancel(const TimerId &timer_id);

	private:
		void WakeUp();

		void Repeat();

		void Refresh(bool lock);

		bool     running_;
		int      wake_up_[2];
		Poller  *poller_;

		Atomic<int32_t> next_time_out_;

		atomic_32_t seq_;

		BoundedQueue<Task>     *queue_;
		ThreadPool<Task>       *pool_;
		Mutex                   mutex_;
		std::map<TimerId, Task> pending_;
};

} // namespace Mushroom

#endif /* _EVENT_BASE_HPP_ */