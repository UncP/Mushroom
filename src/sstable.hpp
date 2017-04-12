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

#include "utility.hpp"

namespace Mushroom {

class SSTable
{
	friend class SSTableManager;
	public:
		SSTable(const BLinkTree *b_link_tree, BlockManager *block_manager, table_t table_no);

		~SSTable();

		void FormKeySlice(KeySlice *slice) const;

	private:
		struct BlockInfo {
			BlockInfo():block_num_(0) { }

			void AppendKeyRange(const Block *block);

			uint32_t                  key_len_;
			uint32_t                  block_num_;
			std::vector<std::string>  smallest_;
			std::vector<std::string>  largest_;
		};

		table_t              table_no_;
		bool                 pin_;
		BlockInfo            info_;
		std::vector<Block *> blocks_;
};

} // namespace Mushroom

#endif /* _SSTABLE_HPP_ */

#endif