/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 14:22:34
**/

#ifndef _MUSHROOM_LOG_HPP_
#define _MUSHROOM_LOG_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class KeySlice;

class Log : private NoCopy
{
	public:
		Log(uint8_t op, const KeySlice *key):op_(op), key_(key) { }

	private:
		uint8_t         op_;
		const KeySlice *key_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_LOG_HPP_ */