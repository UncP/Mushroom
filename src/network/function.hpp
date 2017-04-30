/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 17:25:03
**/

#ifndef _FUNCTIONAL_HPP_
#define _FUNCTIONAL_HPP_

#include <functional>

namespace Mushroom {

typedef std::function<void()> ReadCallBack;
typedef std::function<void()> WriteCallBack;
typedef std::function<void()> SendCallBack;

} // namespace Mushroom

#endif /* _FUNCTIONAL_HPP_ */