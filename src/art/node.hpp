/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 09:42:33
**/

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <cstdint>

namespace Mushroom {

enum NodeType { NODE4 = 0x0, NODE16, NODE48, NODE256, LEAF };

class Node
{
	public:
		uint8_t type_;
		uint8_t count_;
};

class Node4 : public Node
{
	public:
		char    key_[4];
		Node   *child_[4];
};

class Node16 : public Node
{
	public:
		char    key_[16];
		Node   *child_[16];
};

class Node48 : public Node
{
	public:
		char  index_[256];
		Node *child_[48];
};

class Node256 : public Node
{
	public:
		Node *child_[256];
};

class Leaf : public Node
{
	public:

};

} // namespace Mushroom

#endif /* _NODE_HPP_ */