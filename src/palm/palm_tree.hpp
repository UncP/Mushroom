/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-27 17:22:00
**/

#ifndef _PALM_TREE_HPP_
#define _PALM_TREE_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class KeySlice;
class Page;
class PoolManager;
class Barrier;

class PalmTree : private NoCopy
{
	public:
		static const uint32_t MAX_KEY_LENGTH = 255;

		PalmTree(uint32_t key_len, uint32_t threads);

		bool Put(KeySlice *key);

		bool Get(KeySlice *key);

		void Free();

		~PalmTree();

	private:
		struct Set {
			Set():depth_(0) { }
			page_t   page_no_;
			Page    *page_;
			page_t   stack_[8];
			uint32_t depth_;
		};

		PoolManager  *pool_manager_;

		Barrier *barrier_;

		page_t   root_;

		uint8_t  key_len_;
		uint16_t degree_;
};

} // Mushroom

#endif /* _PALM_TREE_HPP_ */