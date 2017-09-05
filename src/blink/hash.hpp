/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-09-05 16:40:16
**/

#ifndef _HASH_HPP_
#define _HASH_HPP_

#include "slice.hpp"

namespace Mushroom {

uint32_t Hash(const char* data, size_t n, uint32_t seed);

inline uint32_t BloomHash(const KeySlice& key) {
  return Hash(key.key_, KeySlice::KeyLen, 0xbc9f1d34);
}

} // namespace Mushroom

#endif /* _HASH_HPP_ */