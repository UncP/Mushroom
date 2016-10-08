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

#include <stdint.h>

namespace Mushroom {

class BTreePage
{
	public:
		static enum { ROOT = 0, BRANCH, LEAF } TYPE;

	private:
		uint8_t   type_;
		uint32_t  page_no_;
		uint16_t  total_key_;		// number of key
		uint16_t  total_chd_;		// number of child == (total_key_ + 1)
		char      data_[];
};

class BTree
{
	public:

		BTree& operator=(const BTree &) = delete;
		BTree(const BTree &) = delete;

	private:
		BTreePage root_;

		uint16_t 	n_;						// B+ 树的度
		uint16_t  key_len_;			// 关键值的长度
		uint16_t 	min_key_;			// 叶子结点允许的最少关键值数
		uint16_t  max_key_;			// 结点中允许的最大关键值数
		uint16_t  min_node_;		// 非叶子结点中最少孩子数
		uint16_t  max_node_;		// 分裂时非叶子结点中存在的孩子结点数

		uint16_t  data_len_;		// 数据长度(键+值)

		void (*copy_)(const void *, const void *, uint32_t);
		int  (*compare_)(const void *, const void *, uint32_t);
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */