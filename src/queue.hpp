/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-17 14:12:46
**/

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include <pthread.h>

#include "utility.hpp"

namespace Mushroom {

class Queue
{
	public:
		Queue(int capacity, uint8_t key_len);

		void Push(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key);

		void Pull();

		void Clear();

		~Queue();

	private:
		bool                    clear_;
		int                     capacity_;
		Task*                  *queue_;
		int                    *avail_;
		int                    *work_;
		int                     front_;
		int                     avail_back_;
		int                     work_back_;
		pthread_mutex_t         mutex_[1];
		pthread_cond_t          ready_[1];
		pthread_cond_t          empty_[1];
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */