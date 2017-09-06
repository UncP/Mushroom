/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-09-05 16:45:40
**/

#ifndef _BLOOM_FILTER_HPP_
#define _BLOOM_FILTER_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class BloomFilter : public NoCopy
{
	public:
		BloomFilter(char *filter, int count, bool clear);

		void Add(const char *data, size_t len);

		bool Match(const char *data, size_t len) const;

		int size() const { return bytes_; }

		static int Size(int count) { return (count * BitsPerKey + 7) >> 3; }

	private:
		static const int BitsPerKey = 10;
		static const int NumProbe   = 0.69 * BitsPerKey;

		char *filter_;
		int   bytes_;
};

} // namespace Mushroom

#endif /* _BLOOM_FILTER_HPP_ */