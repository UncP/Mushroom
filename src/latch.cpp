/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-11-18 10:11:14
**/

#include <cassert>
#include <iostream>

#include "latch.hpp"

namespace Mushroom {

void Latch::Pin()
{
	++users_;
	// pin_ = true;
}

void Latch::UnPin()
{
	--users_;
	// pin_ = false;
}

bool Latch::Busy() const
{
	return users_.load(std::memory_order_relaxed) > 0;
}

void Latch::LockShared()
{
	// if (!pin_) {
	// 	std::cout << pin_ << " " << id_ << " " << users_ << std::endl;
	// 	exit(-1);
	// }
	assert(users_);
	shared_lock_.LockShared();
}

void Latch::UnlockShared()
{
	// if (!pin_) {
	// 	// std::cout << pin_ << " " << id_ << " " << users_ << std::endl;
	// 	exit(-1);
	// }
	assert(users_);
	shared_lock_.UnlockShared();
	UnPin();
}

void Latch::Lock()
{
	// if (!pin_) {
	// 	std::cout << id_ << " " << users_ << std::endl;
	// 	exit(-1);
	// }
	assert(users_);
	shared_lock_.Lock();
}

void Latch::Unlock()
{
	shared_lock_.Unlock();
	UnPin();
}

void Latch::Upgrade()
{
	shared_lock_.Upgrade();
}

void Latch::Downgrade()
{
	shared_lock_.Downgrade();
}

} // namespace Mushroom
