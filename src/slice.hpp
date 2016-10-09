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
		size_t      Len()  const { return len_; }

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

		int compare(const KeySlice *that) const {
			uint8_t min_len = std::min(this->len_, that->len_);
			int res = memcmp(this->data_, that->data_, min_len);
			if (res || this->len_ == that->len_)
				return res;
			return this->len_ > that->len_ ? 1 : -1;
		}

	private:
		uint32_t page_no_;
		uint16_t pos_;
		uint8_t  len_;
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

} // namespace Mushroom

#endif /* _SLICE_HPP_ */