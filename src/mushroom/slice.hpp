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
		std::string ToString(uint8_t len) const {
			return std::string(key_, len) + "\n";
		}
		union {
			page_t  page_no_;
			valptr  vptr_;
		};
		char   key_[0];
};

inline int ComparePrefix(const KeySlice *key, const char *prefix, size_t len) {
	return memcmp(key->key_, prefix, len);
};

inline int CompareSuffix(const KeySlice *a, const KeySlice *b, size_t pre, size_t len)
{
	return memcmp(a->key_ + pre, b->key_, len);
};

inline void CopyPrefix(KeySlice *a, const char *prefix, size_t len) {
	memcpy(a->key_, prefix, len);
};

inline void CopySuffix(KeySlice *a, const char *suffix, size_t pre, size_t len) {
	memcpy(a->key_ + pre, suffix, len);
};

inline void CopyKey(KeySlice *a, const KeySlice *b, size_t pre, size_t len) {
	if (!pre) {
		memcpy(a, b, len + sizeof(valptr));
	} else {
		a->page_no_ = b->page_no_;
		memcpy(a->key_ + pre, b->key_, len);
	}
};

inline KeySlice* NewKeySlice(uint8_t len) {
	char *buf = new char[sizeof(page_t) + len];
	return (KeySlice *)buf;
}

#define TempSlice(name, length)                  \
	char buf_##name[sizeof(page_t) + length];      \
	memset(buf_##name, 0, sizeof(page_t)+length);  \
	KeySlice *name = (KeySlice *)buf_##name;

} // namespace Mushroom

#endif /* _SLICE_HPP_ */