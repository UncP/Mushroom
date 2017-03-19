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
#include <functional>
#include <cassert>

#include "utility.hpp"

namespace Mushroom {

class Slice
{
	public:
		Slice():data_(nullptr), len_(0) { }

		Slice(const char *data, size_t len):data_(data), len_(len) { }

		Slice(const char *data):data_(data), len_(strlen(data)) { }

		const char* Data() const { return data_; }

		size_t Length() const { return len_; }

		Slice& operator=(const Slice &that) = delete;

		std::string ToString() const;

	private:
		const char *data_;
		size_t      len_;
};

class KeySlice
{
	friend inline int ComparePrefix(const KeySlice *key, const char *prefix, size_t len) {
		return memcmp(key->data_, prefix, len);
	};
	friend inline int CompareSuffix(const KeySlice *a, const KeySlice *b, size_t pre, size_t len)
	{
		return memcmp(a->data_ + pre, b->data_, len);
	};
	friend inline void CopyPrefix(KeySlice *a, const char *prefix, size_t len) {
		memcpy(a->data_, prefix, len);
	};
	friend inline void CopyKey(KeySlice *a, const KeySlice *b, size_t pre, size_t len) {
		if (!pre) {
			memcpy(a, b, len + sizeof(page_id));
		} else {
			a->page_no_ = b->page_no_;
			memcpy(a->data_ + pre, b->data_, len);
		}
	};

	public:
		using StringFormat = std::function<std::string(const KeySlice *, uint8_t)>;

		char* Data() { return data_; }
		const char* Data() const { return data_; }

		page_id PageNo() const { return page_no_; }

		void AssignPageNo(page_id page_no) { page_no_ = page_no; }

		static void SetStringFormat(const StringFormat &from_string) {
			form_string_ = from_string;
		}

		std::string ToString(uint8_t len) const {
			return form_string_(this, len);
		}

		static StringFormat form_string_;

	private:
		page_id  page_no_;
		char     data_[0];
};

class DataSlice
{
	public:
		DataSlice() { }

		static const int LengthByte = 2;

		char* Data() { return data_; }
		const char* Data() const { return data_; }

		uint16_t Length() const { return len_; }

		void SetLength(uint16_t len) { len_ = len; }

	private:
		uint16_t len_;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _SLICE_HPP_ */