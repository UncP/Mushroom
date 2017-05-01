/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 22:44:03
**/

#ifndef _SIGNAL_HPP_
#define _SIGNAL_HPP_

#include <csignal>
#include <functional>
#include <unordered_map>

namespace Mushroom {

class Signal
{
	public:
		static void Register(int sig, const std::function<void()> &handler) {
			handlers_[sig] = handler;
			signal(sig, signal_handler);
		}

		static void signal_handler(int sig) {
			handlers_[sig]();
		}

	private:
		static std::unordered_map<int, std::function<void()>> handlers_;
};

std::unordered_map<int, std::function<void()>> Signal::handlers_;

} // namespace Mushroom

#endif /* _SIGNAL_HPP_ */