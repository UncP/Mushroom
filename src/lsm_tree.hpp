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
		LSMTree(int key_len);

		~LSMTree();

		bool Free();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key) const;

		LSMTree(const LSMTree &) = delete;
		LSMTree& operator=(const LSMTree &) = delete;

	private:
		BLinkTree *curr_;
		#ifndef NOLATCH
		Mutex      mutex_;
		#endif
		uint8_t    key_len_;
};

} // namespace Mushroom

#endif /* _LSM_TREE_HPP_ */

#endif