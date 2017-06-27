/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 10:40:44
**/

#include "art.hpp"

namespace Mushroom {

ART::ART() { }

bool ART::Put(const uint8_t *key, uint32_t len, uint64_t val)
{
	return false;
}

bool ART::Get(const uint8_t *key, uint32_t len, uint64_t *val)
{

	return false;
}

Node* ART::Descend(Node *cur, char byte)
{
	union {
		Node4   *p4;
		Node16  *p16;
		Node48  *p48;
		Node256 *p256;
	}p;
	switch (cur->Type()) {
		case NODE4   : p.p4   = (Node4   *)cur; return p.p4  ->Descend(byte); break;
		case NODE16  : p.p16  = (Node16  *)cur; return p.p16 ->Descend(byte); break;
		case NODE48  : p.p48  = (Node48  *)cur; return p.p48 ->Descend(byte); break;
		case NODE256 : p.p256 = (Node256 *)cur; return p.p256->Descend(byte); break;
		default : assert(0);
	}
}

} // namespace Mushroom
