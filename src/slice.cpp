/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 15:47:31
**/

#include "slice.hpp"

namespace Mushroom {

std::function<std::string(const KeySlice *)> KeySlice::form_string_ = nullptr;

// std::string Slice::ToString() const
// {
// 	char len[16];
// 	snprintf(len, 16, "%d  :", (int)len_);
// 	return "len: " + std::string(len) + std::string(data_) + "\n";
// }

} // namespace Mushroom
