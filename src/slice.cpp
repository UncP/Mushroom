/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-09 15:47:31
**/

#include <sstream>

#include "slice.hpp"

namespace Mushroom {

std::function<std::string(const KeySlice *)> KeySlice::form_string_ = nullptr;

std::string Slice::ToString() const
{
	std::ostringstream os;
	os << "len: " << len_ << "  :" << data_ << "\n";
	return os.str();
}

} // namespace Mushroom
