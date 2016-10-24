/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 10:01:51
**/

#include <iostream>
#include <cassert>

#include "shared_lock.hpp"

namespace Mushroom {

void SharedLock::LockShared()
{
	std::unique_lock<std::mutex> lock(mutex_);
	shared_condition_.wait(lock, [this] { return !(exclusive_ | block_shared_); });
	++shared_count_;
	// std::cout << "lock shared\n"; // << shared_count_ << std::endl;
}

void SharedLock::UnlockShared()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(!exclusive_ && shared_count_);
	--shared_count_;
	// std::cout << "unlock shared\n"; // << shared_count_ << std::endl;
	if (!shared_count_) {
		if (upgrade_) {
			// upgrade_ = false;
			// exclusive_ = true;
			upgrade_condition_.notify_one();
		} else {
			block_shared_ = false;
		}
		exclusive_condition_.notify_one();
		shared_condition_.notify_all();
	}
}

void SharedLock::Lock()
{
	std::unique_lock<std::mutex> lock(mutex_);
	block_shared_ = true;
	exclusive_condition_.wait(lock, [this] { return !(shared_count_ || exclusive_); });
	// std::cout << "lock\n"; // << shared_count_ << std::endl;
	assert(!(shared_count_ || exclusive_));
	exclusive_ = true;
}

void SharedLock::Unlock()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(exclusive_ && !shared_count_);
	exclusive_    = false;
	block_shared_ = false;
	// std::cout << "unlock\n"; // << shared_count_ << std::endl;
	assert(!exclusive_ && !shared_count_);
	exclusive_condition_.notify_one();
	shared_condition_.notify_all();
}

void SharedLock::Upgrade()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(shared_count_);
	--shared_count_;
	block_shared_ = true;
	upgrade_ = true;
	upgrade_condition_.wait(lock, [this] { return !(exclusive_ || shared_count_); });
	// std::cout << "upgrade\n"; // << shared_count_ << std::endl;
	assert(!exclusive_ && upgrade_ && !shared_count_);
	upgrade_ = false;
	block_shared_ = false;
	exclusive_ = true;
	assert(!shared_count_ && exclusive_ && !upgrade_);
}

void SharedLock::Downgrade()
{
	std::unique_lock<std::mutex> lock(mutex_);
	assert(exclusive_ && !shared_count_);
	++shared_count_;
	exclusive_ = false;
	assert(!exclusive_ && shared_count_);
	block_shared_ = false;
	exclusive_condition_.notify_one();
	shared_condition_.notify_all();
}

// void SharedLock::Upgrade()
// {
// 	std::unique_lock<std::mutex> lock(mutex_);
// 	shared_condition_.wait(lock, [this] { return !(exclusive_ || upgrade_ || block_shared_); });
// 	// std::cout << "upgrade\n"; // << shared_count_ << std::endl;
// 	++shared_count_;
// 	upgrade_ = true;
// }

// void SharedLock::UnlockUpgrade()
// {
// 	std::unique_lock<std::mutex> lock(mutex_);
// 	upgrade_ = false;
// 	--shared_count_;
// 	if(!shared_count_) {
// 		block_shared_ = false;
// 		exclusive_condition_.notify_one();
// 		shared_condition_.notify_all();
// 	} else {
// 		shared_condition_.notify_all();
// 	}
// }

// void SharedLock::UpgradeToLock()
// {
// 	std::unique_lock<std::mutex> lock(mutex_);
// 	assert(!exclusive_ && upgrade_ && shared_count_);
// 	--shared_count_;
// 	upgrade_condition_.wait(lock, [this] { return !shared_count_; });
// 	upgrade_ = false;
// 	exclusive_ = true;
// 	assert(!shared_count_ && exclusive_ && !upgrade_);
// }

// void SharedLock::LockToShared()
// {
// 	std::unique_lock<std::mutex> lock(mutex_);
// 	assert(exclusive_ && !shared_count_ && !upgrade_);
// 	exclusive_ = false;
// 	++shared_count_;
// 	block_shared_ = false;
// 	exclusive_condition_.notify_one();
// 	shared_condition_.notify_all();
// }

} // namespace Mushroom
