/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-06 22:38:46
**/

#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <functional>

namespace Mushroom {

class NoCopy {
	public:
		NoCopy() = default;
	private:
		NoCopy(const NoCopy &) = delete;
		NoCopy& operator=(const NoCopy &) = delete;
};

template<typename T>
class NoCopyTemplate
{
	public:
		NoCopyTemplate() = default;
	private:
		NoCopyTemplate(const NoCopyTemplate &) = delete;
		NoCopyTemplate& operator=(const NoCopyTemplate &) = delete;
};

typedef uint32_t valptr;
typedef uint32_t page_t;
typedef uint32_t table_t;
typedef uint32_t block_t;

typedef std::function<void()> Task;
typedef std::function<void()> Func;

} // namespace Mushroom

#endif /* _UTILITY_HPP_ */