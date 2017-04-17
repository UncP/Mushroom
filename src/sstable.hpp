/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 14:45:42
**/

#ifndef NOLSM

#ifndef _SSTABLE_HPP_
#define _SSTABLE_HPP_

#include <vector>
#include <string>
#include <cassert>

#include "utility.hpp"
#include "block.hpp"

namespace Mushroom {

class SSTable
{
	friend class SSTableManager;

	public:
		SSTable(table_t table_no, uint32_t level);

		static uint32_t MaxSizeInBytes(uint32_t level);

		bool Append(const Key &key, BlockManager *block_manager);

		const std::vector<Block *>& Blocks() const { return blocks_; }

		void Generate(const BLinkTree *b_link_tree, BlockManager *block_manager);

		void Reset(table_t table_no, uint32_t level);

		class Iterator {
			public:
				Iterator(const SSTable *sstable, uint32_t key_len)
				:key_len_(key_len), curr_(0), blocks_(sstable->Blocks()) {
					assert(blocks_.size());
					iter_ = new Block::Iterator(blocks_[curr_++], key_len_);
				}

				bool Next() {
					if (iter_->Next()) return true;
					else if (curr_ < blocks_.size()) {
						delete iter_;
						iter_ = new Block::Iterator(blocks_[curr_++], key_len_);
						return true;
					} else return false;
				}

				~Iterator() { delete iter_; }

				const Key& key() { return iter_->key_; }

			private:
				uint32_t                    key_len_;
				uint32_t                    curr_;
				Block::Iterator            *iter_;
				const std::vector<Block *> &blocks_;
		};

	private:
		void AppendKeyRange(const Block *block, uint32_t key_len);

		table_t                   table_no_;
		uint32_t                  level_;
		uint32_t                  block_num_;
		std::vector<Block *>      blocks_;
		std::vector<std::string>  smallest_;
		std::vector<std::string>  largest_;
};

} // namespace Mushroom

#endif /* _SSTABLE_HPP_ */

#endif /* NOLSM */