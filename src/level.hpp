/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:54:51
**/

#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include <vector>

#include "utility.hpp"

namespace Mushroom {

class Level
{
	public:
		friend class LevelTree;

		Level(uint32_t level);

		~Level();

		uint32_t SSTableNumber() const;

		bool ReachThreshold();

		void GetKeyRange(Key *smallest, Key *largest) const;

		void FindOverlapSSTable(std::vector<SSTable *> *tables, uint32_t *index, uint32_t *total,
			const Key &smallest, const Key &largest) const;

		SSTable* NextSSTable(Key &offset, uint32_t *index) const;

		void AppendSSTable(SSTable *sstable);

		void DeleteSSTable(uint32_t index, uint32_t total, SSTableManager *sstable_manager);

		bool UpdateSSTable(uint32_t index, uint32_t total, const std::vector<SSTable *> &result,
			SSTableManager *sstable_manager);

		std::vector<SSTable *> SSTables() const;

	private:
		uint32_t               level_;
		std::vector<SSTable *> sstables_;
};

} // namespace Mushroom

#endif /* _LEVEL_HPP_ */