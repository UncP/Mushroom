/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-04 09:18:22
**/

#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <thread>

#include "utility.hpp"

namespace Mushroom {

class Thread : private NoCopy
{
	public:
		Thread(const Func &func):func_(func) { }

		inline void Start() {
			std::thread tmp(func_);
			thread_.swap(tmp);
		}

		inline void Stop() {
			thread_.join();
		}

	private:
		Func        func_;
		std::thread thread_;
};

} // namespace Mushroom

#endif /* _THREAD_HPP_ */