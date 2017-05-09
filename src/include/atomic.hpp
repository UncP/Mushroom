/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-04 12:54:54
**/

#ifndef _ATOMIC_HPP_
#define _ATOMIC_HPP_

#include "utility.hpp"

namespace Mushroom {

template<typename T>
class Atomic : private NoCopyTemplate<T>
{
	public:
		Atomic() { }

		Atomic(T val):val_(val) { }

		inline T get() {
			return __sync_val_compare_and_swap(&val_, 0, 0);
		}

		inline T operator++() {
			return __sync_add_and_fetch(&val_, 1);
		}

		inline T operator--() {
			return __sync_add_and_fetch(&val_, -1);
		}

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
		volatile T val_;
};

typedef Atomic<uint8_t>  atomic_8_t;
typedef Atomic<uint16_t> atomic_16_t;
typedef Atomic<uint32_t> atomic_32_t;

} // namespace Mushroom

#endif /* _ATOMIC_HPP_ */