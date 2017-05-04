/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-04 12:54:54
**/

#ifndef _ATOMIC_HPP_
#define _ATOMIC_HPP_

namespace Mushroom {

template<typename T>
class Atomic
{
	public:
		Atomic(T val):val_(val) { }

		inline T operator++(int) {
			return __sync_fetch_and_add(&val_, 1);
		}

		inline T operator--(int) {
			return __sync_fetch_and_add(&val_, -1);
		}

		inline Atomic& operator=(T new_val) {
			__sync_val_compare_and_swap(&val_, val_, new_val);
			return *this;
		}

	private:
		T val_;
};

} // namespace Mushroom

#endif /* _ATOMIC_HPP_ */