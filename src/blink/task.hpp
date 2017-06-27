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
		MushroomTask():fun_(0), db_(0), key_(NewKeySlice()) { }

		~MushroomTask() { DeleteKeySlice(key_); }

		inline void Assign(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key) {
			fun_ = fun;
			db_  = db;
			memcpy(key_, key, KeySlice::KeySize);
		}

		bool operator()() { return (db_->*fun_)(key_); }

	private:
		bool         (MushroomDB::*(fun_))(KeySlice *);
		MushroomDB   *db_;
		KeySlice     *key_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_TASK_HPP_ */