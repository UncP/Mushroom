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

#include "slice.hpp"
#include "utility.hpp"

namespace Mushroom {

class Block
{
	public:
		friend class BlockManager;

		static const uint32_t BlockSize = 65536;

		Block():mem_(new char[BlockSize]), num_((uint32_t *)mem_), off_(4), next_(0), pin_(true) { }

		const char* Memory() const { return mem_ + 4; }

		uint32_t TotalKey() const { return *num_; }

		~Block() { delete [] mem_; }

		inline bool Append(const char *data, uint32_t len) {
			if (off_ + len < BlockSize) {
				memcpy(mem_ + off_, data, len);
				off_ += len;
				++*num_;
				return true;
			}
			return false;
		}

		class Iterator {
			public:
				Iterator(const Block *block, uint32_t key_len)
				:key_(key_len), mem_(block->Memory()), tot_(block->TotalKey()), idx_(0) {
					memcpy(key_.data_, mem_, key_.size_);
				}

				inline bool Next() {
					if (++idx_ < tot_) {
						memcpy(key_.data_, mem_ + idx_ * key_.size_, key_.size_);
						return true;
					}
					return false;
				}
				inline void First() {
					assert(tot_);
					idx_ = 0;
					memcpy(key_.data_, mem_, key_.size_);
				}
				inline void Last() {
					idx_ = tot_;
					assert(idx_--);
					memcpy(key_.data_, mem_ + idx_ * key_.size_, key_.size_);
				}

				Key          key_;
			private:
				const char  *mem_;
				uint32_t     tot_;
				uint32_t     idx_;
		};

	private:
		char     *mem_;
		uint32_t *num_;
		uint32_t  off_;
		Block    *next_;
		bool      pin_;
};

} // namespace Mushroom

#endif /* _BLOCK_HPP_ */

#endif