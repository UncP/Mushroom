/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-20 18:42:03
**/

#ifndef _ART_HPP_
#define _ART_HPP_

#include "node.hpp"

namespace Mushroom {

class ART
{
	public:
		ART();

		bool Put(const KeySlice *key);

		bool Get(KeySlice *key);

	private:
		Node *root_;
};

} // namespace Mushroom

#endif /* _ART_HPP_ */