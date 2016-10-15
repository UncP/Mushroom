/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-15 10:37:54
**/

#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <iostream>

template<typename T>
void Output(const T *t, bool new_line = false) {
	std::cout << t->ToString();
	if (new_line)
		std::cout << std::endl;
}

#endif /* _UTILITY_HPP_ */
