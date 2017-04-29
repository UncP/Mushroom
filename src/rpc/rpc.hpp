/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:22:47
**/

#ifndef _RPC_HPP_
#define _RPC_HPP_

#include <string>
#include <map>

namespace Mushroom {

template<typename T1, typename T2, typename T3>
class RPC
{
	public:
		RPC() { }

		void Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
			services_[std::string(str)] = Service(obj, fun);
		}

		bool Call(const std::string &str, const T2 *args, T3 *reply) {
			Service &service = services_[str];
		}

		struct Service {
			Service() { }

			Service(T1 *obj, void (T1::*(fun))(const T2*, T3*)):obj_(obj), fun_(fun) { }

			T1 *obj_;
			void (T1::*(fun_))(const T2*, T3*);
		};

	private:
		std::map<std::string, Service> services_;
};

} // namespace Mushroom

#endif /* _RPC_HPP_ */