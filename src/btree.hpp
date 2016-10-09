/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-07 20:10:30
**/

#ifndef _BTREE_HPP_
#define _BTREE_HPP_

#include <cstdint>
#include <iostream>
#include <string>

#include "status.hpp"
#include "slice.hpp"
#include "bt_page.hpp"

namespace Mushroom {

class BTree
{
	public:
		BTree() { }

		Status Init(int max_key_len);

		Status Put(const Slice &key, const Slice &val);
		Status Delete(const Slice &key);
		Status Get(const Slice &key, Slice &val);

		std::string ToString() const;

		BTree& operator=(const BTree &) = delete;
		BTree(const BTree &) = delete;

	private:
		BTreePage root_;

		BTreePager  *pager_;

		uint16_t 	degree_;			// B+ 树的阶
		uint16_t 	min_key_;			// 叶子结点允许的最少关键值数
		uint16_t  max_key_;			// 叶子结点中允许的最大关键值数
		uint16_t  min_node_;		// 非叶子结点中最少孩子数
		uint16_t  max_node_;		// 分裂时非叶子结点中存在的孩子结点数

		uint8_t   max_key_len_;
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */