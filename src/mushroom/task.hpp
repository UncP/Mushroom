/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time: 2016-10-18 15:30:39
**/

#ifndef _MUSHROOM_TASK_HPP_
#define _MUSHROOM_TASK_HPP_

#include "../include/utility.hpp"
#include "slice.hpp"

namespace Mushroom {

class MushroomDB;

class MushroomTask : private NoCopy
{
	public:
		MushroomTask(uint8_t key_len):fun_(0), db_(0), key_len_(key_len) {
			char *buf = new char[sizeof(valptr) + key_len_];
			key_ = (KeySlice *)buf;
		}

		~MushroomTask() { delete [] key_; }

		inline void Assign(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key) {
			fun_ = fun;
			db_  = db;
			CopyKey(key_, key, 0, sizeof(valptr) + key_len_);
		}

		bool operator()() { return (db_->*fun_)(key_); }

	private:
		bool         (MushroomDB::*(fun_))(KeySlice *);
		MushroomDB   *db_;
		KeySlice     *key_;
		uint8_t       key_len_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_TASK_HPP_ */