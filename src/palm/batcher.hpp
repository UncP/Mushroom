/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-28 16:46:00
**/

#ifndef _BATCHER_HPP_
#define _BATCHER_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class Page;

// batcher is just a wrapper of page, we use it to store keys in order before
// divide them into batch
class Batcher : private NoCopy
{
	public:
		Batcher();

	private:

};

} // Mushroom

#endif /* _BATCHER_HPP_ */