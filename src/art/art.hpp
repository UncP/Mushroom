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

		bool Put(const uint8_t *key, uint32_t len, uint32_t val);

		bool Get(const uint8_t *key, uint32_t len, uint32_t *val);

	private:
		Node** Descend(Node *cur, char byte);

		bool Insert(Node *cur, Node **ref, const uint8_t *key, uint32_t depth, uint32_t len,
			uint32_t val);

		Node *root_;
};

} // namespace Mushroom

#endif /* _ART_HPP_ */