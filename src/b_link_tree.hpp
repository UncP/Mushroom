/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:10:30
**/

#ifndef _B_LINK_TREE_HPP_
#define _B_LINK_TREE_HPP_

#include "utility.hpp"
#include "latch.hpp"

namespace Mushroom {

class BLinkTree
{
	public:
		class Iterator
		{
			public:
				Iterator(const BLinkTree *b_link_tree, int32_t level = 0);
				~Iterator();
				inline bool Begin();
				inline bool Next();

				KeySlice        *key_;
			private:
				const BLinkTree *b_link_tree_;
				Page            *curr_;
				int32_t          level_;
				uint16_t         index_;
		};

		static const uint32_t MAX_KEY_LENGTH = 255;

		#ifndef NOLATCH
		BLinkTree(int key_len, LatchManager *latch_manager, PoolManager *page_manager);
		#else
		BLinkTree(int key_len, PoolManager *page_manager);
		#endif
		~BLinkTree();

		void Initialize();

		bool Free();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key) const;

		#ifdef LSM
		inline bool NeedCompact() const;
		bool Clear();
		#endif

		BLinkTree(const BLinkTree &) = delete;
		BLinkTree& operator=(const BLinkTree &) = delete;

	private:

		bool First(Page **page, int32_t level) const;

		bool Next(KeySlice *key, Page **page, uint16_t *index) const;

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
		#ifdef LSM
		inline void Ref() { __sync_fetch_and_add(&ref_, 1); }
		inline void Unref() {
			__sync_fetch_and_add(&ref_, -1);
			if (!ref_) cond_.Signal();
		}

		uint32_t          ref_;
		Mutex             mutex_;
		ConditionVariable cond_;
		#endif
		LatchManager     *latch_manager_;
		#endif

		PoolManager  *page_manager_;
		page_id       root_;

		uint8_t       key_len_;
		uint16_t      degree_;
};

} // namespace Mushroom

#endif /* _B_LINK_TREE_HPP_ */