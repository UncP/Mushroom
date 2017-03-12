/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-09 16:04:16
**/

#ifndef _STATUS_HPP_
#define _STATUS_HPP_

#include <cstdint>

namespace Mushroom {

typedef uint32_t page_id;

enum Status {Fail = 0, Success, Fatal };

} // namespace Mushroom

#endif /* _STATUS_HPP_ */