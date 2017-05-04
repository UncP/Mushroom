/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:08:40
**/

#ifndef _LSM_TREE_HPP_
#define _LSM_TREE_HPP_

#include "utility.hpp"
#include "../utility/latch.hpp"

namespace Mushroom {

class LSMTree
{
	public:
		LSMTree(uint32_t key_len);

		~LSMTree();

		bool Free();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key) const;

		LSMTree(const LSMTree &) = delete;
		LSMTree& operator=(const LSMTree &) = delete;

	private:
		void SwitchMemoryTree();

		uint32_t      key_len_;
		BLinkTree    *mem_tree_;
		BLinkTree    *imm_tree_;

		LevelTree    *lvl_tree_;

		#ifndef NOLATCH
		Mutex             mutex_;
		SpinLatch         spin_;
		#endif
};

} // namespace Mushroom

#endif /* _LSM_TREE_HPP_ */