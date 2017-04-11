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
	public:
		SSTable(const BLinkTree *b_link_tree);

		~SSTable();

		void FormKeySlice(KeySlice *slice) const;

	private:
		struct Info {
			Info();

			void AppendKeyRange(const Block *block);

			uint32_t                  key_len_;
			uint32_t                  block_size_;
			std::vector<std::string>  smallest_;
			std::vector<std::string>  largest_;
		};

		Info                 info_;
		std::vector<Block *> blocks_;
};

} // namespace Mushroom

#endif /* _SSTABLE_HPP_ */

#endif