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
		BloomFilter(int count);

		~BloomFilter();

		void Add(const char *data, size_t len);

		bool Match(const char *data, size_t len) const;

		int size() const { return bytes_; }

	private:
		static const int BitsPerKey = 10;
		static const int NumProbe   = BitsPerKey * 0.69;
		int   bytes_;
		char *filter_;
};

} // namespace Mushroom

#endif /* _BLOOM_FILTER_HPP_ */