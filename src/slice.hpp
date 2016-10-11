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

typedef uint32_t page_id;

class Slice
{
	friend
		std::ostream& operator<<(std::ostream &os, const Slice *slice) {
			return os << slice->ToString();
		};

	public:
		Slice():data_(nullptr), len_(0) { }

		Slice(const char *data, size_t len):data_(data), len_(len) { }

		Slice(const char *data):data_(data), len_(strlen(data)) { }

		const char* Data() const { return data_; }
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
	public:

		KeySlice() { }

		const char* Data() const { return data_; }

		page_id PageNo() const { return page_no_; }

		bool DataEmpty() const { int i = 0; return memcmp(this, &i, 4) == 0; }

	private:
		page_id  page_no_;
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

template<typename T1, typename T2>
inline int compare(const T1 *a, const T2 *b, size_t len) {
	return memcmp(a->Data(), b->Data(), len);
}

} // namespace Mushroom

#endif /* _SLICE_HPP_ */