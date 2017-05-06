/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-17 14:12:46
**/

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include "../utility/utility.hpp"
#include "utility.hpp"
#include "../utility/mutex.hpp"
#include "../utility/cond.hpp"

namespace Mushroom {

class MushroomDB;
class KeySlice;
class Task;

class Queue : private NoCopy
{
	public:
		Queue(int capacity, uint8_t key_len);

		void Push(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key);

		void Pull();

		void Clear();

		~Queue();

	private:
		bool   clear_;
		int    capacity_;
		Task* *queue_;
		int   *avail_;
		int   *work_;
		int    front_;
		int    avail_back_;
		int    work_back_;
		Mutex  mutex_;
		Cond   ready_;
		Cond   empty_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */