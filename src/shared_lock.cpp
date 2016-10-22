/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 10:01:51
**/

#include <cassert>

#include "shared_lock.hpp"

namespace Mushroom {

void SharedLock::LockShared()
{
	std::unique_lock<std::mutex> lock(mutex_);
	shared_condition_.wait(lock, [this]{ return !(exclusive_ | block_shared_); });
	++shared_count_;
	std::cout << "lock shared\n"; // << shared_count_ << std::endl;
}

void SharedLock::UnlockShared()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(!exclusive_ && shared_count_);
	--shared_count_;
	if (!shared_count_ && block_shared_) {
		// block_shared_ = false;
		exclusive_condition_.notify_one();
	}
	std::cout << "unlock shared\n"; // << shared_count_ << std::endl;
}

void SharedLock::Lock()
{
	std::unique_lock<std::mutex> lock(mutex_);
	block_shared_ = true;
	if (shared_count_) {
		exclusive_condition_.wait(lock, [this]{
			return !(shared_count_ || exclusive_); });
	}
	block_shared_ = false;
	std::cout << "lock\n"; // << shared_count_ << std::endl;
	assert(!shared_count_ && !exclusive_ && !block_shared_);
	exclusive_ = true;
}

void SharedLock::Unlock()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(exclusive_ && !shared_count_);
	block_shared_ = false;
	exclusive_    = false;
	exclusive_condition_.notify_one();
	shared_condition_.notify_all();
	std::cout << "unlock\n"; // << shared_count_ << std::endl;
}

void SharedLock::Upgrade()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(shared_count_);
	block_shared_ = true;
	--shared_count_;
	if (shared_count_) {
		exclusive_condition_.wait(lock, [this]{ return !(shared_count_ || exclusive_); });
	}
	block_shared_ = false;
	assert(!shared_count_ && !exclusive_ && !block_shared_);
	std::cout << "upgrade\n"; // << shared_count_ << std::endl;
	exclusive_ = true;
}

} // namespace Mushroom
