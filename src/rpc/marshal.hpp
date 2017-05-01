/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 09:17:51
**/

#ifndef _MARSHAL_HPP_
#define _MARSHAL_HPP_

#include <cstring>
#include <vector>

#include "../network/buffer.hpp"

namespace Mushroom {

class Marshal
{
	public:
		Marshal(Buffer &input, Buffer &output);

		~Marshal();

		void Read(const void *str, uint32_t len);

		void Write(void *str, uint32_t len);

	private:
		Buffer &input_;
		Buffer &output_;
};

inline Marshal& operator<<(Marshal &marshal, const char *str) {
	marshal.Read(str, strlen(str));
	return marshal;
}

inline Marshal& operator<<(Marshal &marshal, const int32_t &v) {
	marshal.Read(&v, 4);
	return marshal;
}

inline Marshal& operator<<(Marshal &marshal, const uint32_t &v) {
	marshal.Read(&v, 4);
	return marshal;
}

template<typename T>
inline Marshal& operator<<(Marshal &marshal, const std::vector<T> &v) {
	uint32_t e = v.size();
	marshal << e;
	for (uint32_t i = 0; i < e; ++i)
		marshal << v[i];
	return marshal;
}


inline Marshal& operator>>(Marshal &marshal, char *str) {
	marshal.Write(str, strlen(str));
	return marshal;
}

inline Marshal& operator>>(Marshal &marshal, int32_t &v) {
	marshal.Write(&v, 4);
	return marshal;
}

inline Marshal& operator>>(Marshal &marshal, uint32_t &v) {
	marshal.Write(&v, 4);
	return marshal;
}

template<typename T>
inline Marshal& operator>>(Marshal &marshal, std::vector<T> &v) {
	uint32_t e;
	marshal >> e;
	v.reserve(e);
	for (uint32_t i = 0; i < e; ++i) {
		T t;
		marshal >> t;
		v.push_back(t);
	}
	return marshal;
}

} // namespace Mushroom

#endif /* _MARSHAL_HPP_ */