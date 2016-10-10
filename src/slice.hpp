/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 15:37:50
**/

#ifndef _SLICE_HPP_
#define _SLICE_HPP_

#include <cstring>
#include <string>
#include <iostream>

namespace Mushroom {

class Slice
{
	friend
		std::ostream& operator<<(std::ostream &os, const Slice *slice) {
			return os << slice->ToString();
		}

	public:
		Slice():data_(nullptr), len_(0) { }

		Slice(const char *data, size_t len):data_(data), len_(len) { }

		Slice(const char *data):data_(data), len_(strlen(data)) { }

		const void* Data() const { return data_; }
		size_t      Length()  const { return len_; }

		bool Empty() const { return !len_; }

		Slice& operator=(const Slice &that) = default;

		std::string ToString() const;

	private:
		const char *data_;
		size_t      len_;
};

class KeySlice
{
	friend
		inline int compare(const KeySlice *a, const KeySlice *b, uint8_t len) {
			return memcmp(a->data_, b->data_, (size_t)len);
		}

	public:

		KeySlice() { }

		const char* Key() const { return data_; }

		bool DataEmpty() const { int i = 0; return memcmp(this, &i, 4) == 0; }

	private:
		unsigned page_no_:22;
		unsigned pos_:10;

		char     data_[0];
};

class DataSlice
{
	public:
		DataSlice() { }

	private:
		uint16_t len_;
		char     data_[0];
};

void KeyCopy(KeySlice *a, KeySlice *b, uint8_t len);

} // namespace Mushroom

#endif /* _SLICE_HPP_ */