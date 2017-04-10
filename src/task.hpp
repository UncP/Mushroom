/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time: 2016-10-18 15:30:39
**/

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "utility.hpp"
#include "slice.hpp"

namespace Mushroom {

class Task
{
	public:
		Task(uint8_t key_len):fun_(0), db_(0), key_len_(key_len) {
			char *buf = new char[sizeof(valptr) + key_len_];
			key_ = (KeySlice *)buf;
		}

		void Assign(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key) {
			fun_ = fun;
			db_  = db;
			CopyKey(key_, key, 0, key_len_);
		}

		bool operator()() { return (db_->*fun_)(key_); }

		Task(const Task &) = delete;
		Task& operator=(const Task &) = delete;

		~Task() { delete [] key_; }

	private:
		bool         (MushroomDB::*(fun_))(KeySlice *);
		MushroomDB   *db_;
		KeySlice     *key_;
		uint8_t       key_len_;
};

} // namespace Mushroom

#endif /* _TASK_HPP_ */