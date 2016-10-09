/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-07 20:12:13
**/

#include "btree.hpp"

namespace Mushroom {

std::string BTreePage::ToString() const
{
	std::string str("type: ");
	if (type_ == LEAF)   str += "leaf  ";
	if (type_ == BRANCH) str += "branch  ";
	if (type_ == ROOT)   str += "root  ";
	char no[16];
	snprintf(no, 16, "%d  ", page_no_);
	str += "page_no: " + std::string(no);
	snprintf(no, 16, "%d  ", total_key_);
	str += "tot_key: " + std::string(no);
	snprintf(no, 16, "%d  ", total_child_);
	str += "tot_chd: " + std::string(no);
	snprintf(no, 16, "%d  ", right_);
	str += "right: " + std::string(no) + "\n";
	return std::move(str);
}

std::string BTree::ToString() const
{
	std::string str;
	char no[16];
	snprintf(no, 16, "%d  ", degree_);
	str = "阶: " + std::string(no);
	snprintf(no, 16, "%d  ", min_key_);
	str = "min_key: " + std::string(no);
	snprintf(no, 16, "%d  ", max_key_);
	str = "max_key: " + std::string(no);
	snprintf(no, 16, "%d  ", min_node_);
	str = "min_node: " + std::string(no);
	snprintf(no, 16, "%d  ", max_node_);
	str = "max_node: " + std::string(no) + "\n";
	return std::move(str);
}

Status BTree::Init(int max_key_len)
{
	// TODO

	max_key_len_ = static_cast<uint8_t>(max_key_len);

	int degree = (BTreePage::PageSize - 13) / (BTreePage::DataIdLength + 2 + 1 + max_key_len) + 1;
	degree_ = static_cast<uint16_t>(degree);

	return Success;
}

} // namespace Mushroom
