/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-09-05 16:53:25
**/

#include <cstring>

#include "bloom.hpp"

namespace Mushroom {

uint32_t Hash(const char *data, size_t n, uint32_t seed)
{
	// Similar to murmur hash
	const uint32_t m = 0xc6a4a793;
	const uint32_t r = 24;
	const char* limit = data + n;
	uint32_t h = static_cast<uint32_t>(seed ^ (n * m));

	// Pick up four bytes at a time
	while (data + 4 <= limit) {
		uint32_t w;
		memcpy(&w, data, sizeof(uint32_t));
		data += 4;
		h += w;
		h *= m;
		h ^= (h >> 16);
	}

	// Pick up remaining bytes
	switch (limit - data) {
		// Note: The original hash implementation used data[i] << shift, which
		// promotes the char to int and then performs the shift. If the char is
		// negative, the shift is undefined behavior in C++. The hash algorithm is
		// part of the format definition, so we cannot change it; to obtain the same
		// behavior in a legal way we just cast to uint32_t, which will do
		// sign-extension. To guarantee compatibility with architectures where chars
		// are unsigned we first cast the char to int8_t.
		case 3:
			h += static_cast<uint32_t>(static_cast<int8_t>(data[2])) << 16;
		// fall through
		case 2:
			h += static_cast<uint32_t>(static_cast<int8_t>(data[1])) << 8;
		// fall through
		case 1:
			h += static_cast<uint32_t>(static_cast<int8_t>(data[0]));
			h *= m;
			h ^= (h >> r);
			break;
	}
	return h;
}

inline uint32_t BloomHash(const char *data, size_t len)
{
	return Hash(data, len, 0xbc9f1d34);
}

BloomFilter::BloomFilter(char *filter, int count, bool clear):filter_(filter)
{
	bytes_ = Size(count);
	if (clear) memset(filter_, 0, bytes_);
}

void BloomFilter::Add(const char *data, size_t len)
{
  int bits = bytes_ << 3;
  uint32_t h = BloomHash(data, len);
  const uint32_t delta = (h >> 17) | (h << 15);
  for (int j = 0; j < NumProbe; ++j) {
    const uint32_t bitpos = h % bits;
    filter_[bitpos / 8] |= (1 << (bitpos % 8));
    h += delta;
  }
}

bool BloomFilter::Match(const char *key, size_t len) const
{
  int bits = bytes_ << 3;
  uint32_t h = BloomHash(key, len);
  const uint32_t delta = (h >> 17) | (h << 15);
  for (int j = 0; j < NumProbe; ++j) {
    const uint32_t bitpos = h % bits;
    if ((filter_[bitpos / 8] & (1 << (bitpos % 8))) == 0) return false;
    h += delta;
  }
  return true;
}

} // namespace Mushroom
