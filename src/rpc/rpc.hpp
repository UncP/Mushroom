/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:22:47
**/

#ifndef _RPC_HPP_
#define _RPC_HPP_

namespace Mushroom {

template<typename T1, typename T2, typename T3>
class RPC
{
	public:
		RPC(T1 *obj, void (T1::*(fun))(const T2 *, T3 *)) {
			service_ = [=](Marshal &in, Marshal &out) {
				T2 args;
				T3 reply;
				in >> args;
				(obj->*fun)(&args, &reply);
				out << reply;
			}
		}

		void operator()(T2, T3)
	private:
		std::function<void()> service_;
};

} // namespace Mushroom

#endif /* _RPC_HPP_ */