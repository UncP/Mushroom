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
  	BloomFilter(int bits_per_key);

  	void CreateFilter(const Slice* keys, int n, std::string* dst) const;

  	bool KeyMayMatch(const Slice& key, const Slice& bloom_filter) const;

  private:
    size_t  bits_per_key_;
    size_t  num_probes_;
    size_t  len_;
    char   *filter_;
};

} // namespace Mushroom

#endif /* _BLOOM_FILTER_HPP_ */