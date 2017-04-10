/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:32
**/

#ifdef LSM

#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_

#include <cassert>
#include <cstring>

#include "utility.hpp"

namespace Mushroom {

class Block
{
	public:
		static const uint32_t BlockSize = 65536;
		class Iterator {
			public:
				Iterator(const Block *block);

				inline bool Next() {
					if (++idx_ < block_->TotalKey()) {
						key_ += block_->KeyLength();
						return true;
					}
					return false;
				}
				inline void First() {
					assert(block_->TotalKey());
					idx_ = 0;
					key_ = block_->mem_ + 4;
				}
				inline void Last() {
					idx_ = block_->TotalKey();
					assert(idx_);
					--idx_;
					key_ = block_->mem_ + 4 + idx_ * block_->KeyLength();
				}

				char    *key_;

			private:
				const Block *block_;
				uint32_t     idx_;
		};

		Block(uint32_t key_len);

		inline uint32_t KeyLength() const { return key_len_; }

		inline uint32_t TotalKey() const { return *num_; }

		~Block();

		inline bool Append(const char *data) {
			if (off_ + key_len_ < BlockSize) {
				memcpy(mem_ + off_, data, key_len_);
				off_ += key_len_;
				++*num_;
				return true;
			}
			return false;
		}

	private:
		char     *mem_;
		uint32_t *num_;
		uint32_t  off_;
		uint32_t  key_len_;
};

} // namespace Mushroom

#endif /* _BLOCK_HPP_ */

#endif