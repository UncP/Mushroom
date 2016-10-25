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

#include "status.hpp"

namespace Mushroom {

class KeySlice;

// class Slice
// {
// 	public:
// 		Slice():data_(nullptr), len_(0) { }

// 		Slice(const char *data, size_t len):data_(data), len_(len) { }

// 		Slice(const char *data):data_(data), len_(strlen(data)) { }

// 		const char* Data() const { return data_; }
// 		size_t      Length()  const { return len_; }

// 		bool Empty() const { return !len_; }

// 		Slice& operator=(const Slice &that) = default;

// 		std::string ToString() const;

// 	private:
// 		const char *data_;
// 		size_t      len_;
// };

class KeySlice
{
	friend inline int CompareKey(const KeySlice *a, const KeySlice *b, size_t len) {
		return memcmp(a->data_, b->data_, len);
	};
	friend inline void CopyKey(KeySlice *a, const KeySlice *b, size_t len) {
		memcpy(a, b, len);
	};

	public:
		using StringFormat = std::function<std::string(const KeySlice *)>;

		KeySlice() { }

		const char* Data() const { return data_; }

		std::string ToString() const {
			return form_string_(this);
		}

		char* Data() { return data_; }

		page_id PageNo() const { return page_no_; }

		page_id Valid() const { return page_no_ != 0; }

		void AssignPageNo(page_id page_no) { page_no_ = page_no; }

		void Assign(page_id page_no, const char *data, size_t len) {
			page_no_ = page_no;
			memcpy(data_, data, len);
		}

		static void SetStringFormat(const StringFormat &from_string) {
			form_string_ = from_string;
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

		uint16_t Length() const { return len_; }

	private:
		uint16_t len_;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _SLICE_HPP_ */