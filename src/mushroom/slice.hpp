/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-09 15:37:50
**/

#ifndef _SLICE_HPP_
#define _SLICE_HPP_

#include <cstring>
#include <string>

#include "utility.hpp"

namespace Mushroom {

class Key
{
	public:
		Key():size_(0), data_(0) { }

		Key(uint32_t size):size_(size), data_(new char[size_]) { memset(data_, 0, size_); }

		Key(const Key &key):size_(key.size_) {
			data_ = new char[size_];
			memcpy(data_, key.data_, size_);
		}
		Key& operator=(const Key &key) {
			if (size_ != key.size_) {
				delete [] data_;
				size_ = key.size_;
				data_ = new char[size_];
			}
			memcpy(data_, key.data_, size_);
			return *this;
		}

		bool operator<(const Key &key) const {
			return memcmp(data_, key.data_, size_) < 0;
		}
		bool operator>(const Key &key) const {
			return memcmp(data_, key.data_, size_) > 0;
		}

		std::string ToString() const { return std::string(data_, size_) + "\n"; }

		~Key() { delete [] data_; }

		uint32_t size_;
		char    *data_;
};

class KeySlice
{
	public:
		std::string ToString(uint8_t len) const {
			return std::string(key_, len) + "\n";
		}
		union {
			page_t  page_no_;
			valptr  vptr_;
			table_t tptr_;
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

#define TempSlice(name, length) \
	char buf_##name[sizeof(valptr) + length]; \
	memset(buf_##name, 0, sizeof(valptr)+length);            \
	KeySlice *name = (KeySlice *)buf_##name;

} // namespace Mushroom

#endif /* _SLICE_HPP_ */