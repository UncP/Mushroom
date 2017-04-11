/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:08:40
**/

#ifndef NOLSM

#ifndef _LSM_TREE_HPP_
#define _LSM_TREE_HPP_

#include "utility.hpp"
#ifndef NOLATCH
#include "latch.hpp"
#endif

namespace Mushroom {

class LSMTree
{
	public:
		LSMTree(int component, int key_len);

		~LSMTree();

		bool Free();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key) const;

		LSMTree(const LSMTree &) = delete;
		LSMTree& operator=(const LSMTree &) = delete;

	private:
		void Merge(const SSTable *table);

		uint32_t    component_;
		uint32_t    key_len_;
		uint32_t    curr_;
		BLinkTree **trees_;

		#ifndef NOLATCH
		Mutex      mutex_;
		#endif
};

} // namespace Mushroom

#endif /* _LSM_TREE_HPP_ */

#endif