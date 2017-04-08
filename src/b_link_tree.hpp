/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:10:30
**/

#ifndef _B_LINK_TREE_HPP_
#define _B_LINK_TREE_HPP_

#include "utility.hpp"

namespace Mushroom {

class BLinkTree
{
	public:
		static const uint32_t MAX_KEY_LENGTH = 255;

		#ifndef NOLATCH
		BLinkTree(int key_len, LatchManager *latch_manager, PoolManager *page_manager);
		#else
		BLinkTree(int key_len, PoolManager *page_manager);
		#endif

		void Initialize();

		bool Free();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key) const;

		Page* First(page_id *page_no, int level) const;

		bool Next(KeySlice *key, page_id *page_no, uint16_t *index) const;

		BLinkTree(const BLinkTree &) = delete;
		BLinkTree& operator=(const BLinkTree &) = delete;

		~BLinkTree();

	private:
		struct Set {
			Set():depth_(0) { }
			page_id   page_no_;
			#ifndef NOLATCH
			Latch    *latch_;
			#endif
			Page     *page_;
			page_id   stack_[8];
			uint8_t   depth_;
		};

		void DescendToLeaf(const KeySlice *key, Set &set) const;

		void SplitRoot(Set &set);

		void Insert(Set &set, KeySlice *key);

		#ifndef NOLATCH
		LatchManager *latch_manager_;
		#endif
		PoolManager  *page_manager_;

		page_id       root_;

		uint8_t       key_len_;
		uint16_t      degree_;
};

} // namespace Mushroom

#endif /* _B_LINK_TREE_HPP_ */