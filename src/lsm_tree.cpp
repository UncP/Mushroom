/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:22:35
**/

#ifndef NOLSM

#include <cstring>

#include "lsm_tree.hpp"
#include "b_link_tree.hpp"
#include "sstable.hpp"
#include "block_manager.hpp"
#include "sstable_manager.hpp"
#include "merger.hpp"

namespace Mushroom {

LSMTree::LSMTree(uint32_t component, uint32_t key_len)
:component_(component - 1), key_len_(key_len), mem_tree_(new BLinkTree(key_len_)),
 imm_tree_(0), disk_trees_(new BLinkTree*[component_]), block_manager_(new BlockManager()),
 sstable_manager_(new SSTableManager())
{
	#ifndef NOLATCH
	imm_pinned_ = false;
	#endif
}

LSMTree::~LSMTree()
{
	delete mem_tree_;
	if (imm_tree_)
		delete imm_tree_;

	delete [] disk_trees_;

	delete block_manager_;
	delete sstable_manager_;
}

bool LSMTree::Free()
{
	mem_tree_->Free();
	if (imm_tree_)
		imm_tree_->Free();

	return true;
}

bool LSMTree::Put(KeySlice *key)
{
	if (mem_tree_->ReachThreshold()) Switch();
	return mem_tree_->Put(key);
}

bool LSMTree::Get(KeySlice *key) const
{
	if (!mem_tree_->Get(key))
		if (imm_tree_ && !imm_tree_->Get(key))
			return false;
	return true;
}

void LSMTree::Switch()
{
	#ifndef NOLATCH
	spin_.Lock();
	#endif
	if (mem_tree_->ReachThreshold()) {
		BLinkTree *new_tree = imm_tree_;
		#ifndef NOLATCH
		mutex_.Lock();
		while (imm_pinned_) cond_.Wait(&mutex_);
		#endif
		imm_tree_ = mem_tree_;
		if (!new_tree) {
			mem_tree_ = new BLinkTree(key_len_);
		} else {
			new_tree->Reset();
			mem_tree_ = new_tree;
		}
		#ifndef NOLATCH
		spin_.Unlock();
		imm_pinned_ = true;
		#endif
		imm_tree_->Clear();
		SSTable *sstable = sstable_manager_->NewSSTable(imm_tree_, block_manager_);
		// imm_tree_->Reset();
		direct_tables_[current_direct_table_++] = sstable->TableNo();
		if (current_direct_table_ == MaxDirectSSTable) {
			table_t copy[MaxDirectSSTable];
			memcpy(copy, direct_tables_, sizeof(table_t) * MaxDirectSSTable);
			current_direct_table_ = 0;
			#ifndef NOLATCH
			imm_pinned_ = false;
			mutex_.Unlock();
			cond_.Signal();
			#endif
			Merge(copy);
		} else {
			#ifndef NOLATCH
			imm_pinned_ = false;
			mutex_.Unlock();
			cond_.Signal();
			#endif
		}
	} else {
		#ifndef NOLATCH
		spin_.Unlock();
		#endif
	}
}

void LSMTree::Merge(const table_t *tables)
{
	Merger m(tables, MaxDirectSSTable, sstable_manager_);
	m.Merge();

	// TempSlice(slice, (key_len_ << 1));
	// table->FormKeySlice(slice);
	// disk_trees_[0]->RangeGet()
}

} // namespace Mushroom

#endif
