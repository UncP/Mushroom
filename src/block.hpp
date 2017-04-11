/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:32
**/

#ifndef NOLSM

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
				Iterator(const Block *block, uint32_t key_len);

				inline bool Next() {
					if (++idx_ < block_->TotalKey()) {
						key_ += key_len_;
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
					key_ = block_->mem_ + 4 + idx_ * key_len_;
				}

				char    *key_;

			private:
				const Block *block_;
				uint32_t     idx_;
				uint32_t     key_len_;
		};

		Block();

		inline uint32_t TotalKey() const { return *num_; }

		~Block();

		inline bool Append(const char *data, uint32_t len) {
			if (off_ + len < BlockSize) {
				memcpy(mem_ + off_, data, len);
				off_ += len;
				++*num_;
				return true;
			}
			return false;
		}

	private:
		char     *mem_;
		uint32_t *num_;
		uint32_t  off_;
};

} // namespace Mushroom

#endif /* _BLOCK_HPP_ */

#endif