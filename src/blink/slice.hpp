/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-09 15:37:50
**/

#ifndef _SLICE_HPP_
#define _SLICE_HPP_

#include <cstring>
#include <string>

#include "../include/utility.hpp"

namespace Mushroom {

class KeySlice : private NoCopy
{
	public:
		static const uint32_t KeyLen  = 16;
		static const uint32_t ValLen  = sizeof(page_t);
		static const uint32_t KeySize = KeyLen + ValLen;

		std::string ToString(uint32_t len = KeyLen) const {
			return std::string(key_, len) + "\n";
		}

		page_t  page_no_;
		char    key_[0];
};

inline KeySlice* NewKeySlice() {
	return (KeySlice *)new char[KeySlice::KeySize];
}

inline void DeleteKeySlice(KeySlice *key) {
	delete [] (char *)key;
}

#define TempSlice(name)                      \
	char buf_##name[KeySlice::KeySize];        \
	memset(buf_##name, 0, KeySlice::KeySize);  \
	KeySlice *name = (KeySlice *)buf_##name;

} // namespace Mushroom

#endif /* _SLICE_HPP_ */