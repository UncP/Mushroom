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
		char* Data() { return data_; }
		const char* Data() const { return data_; }

		page_id PageNo() const { return page_no_; }

		void AssignPageNo(page_id page_no) { page_no_ = page_no; }

		std::string ToString(uint8_t len) const {
			return std::string(data_, len) + "\n";
		}

	private:
		page_id  page_no_;
		char     data_[0];
};

} // namespace Mushroom

#endif /* _SLICE_HPP_ */