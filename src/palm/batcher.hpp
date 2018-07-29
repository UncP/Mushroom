/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-28 16:46:00
**/

#ifndef _BATCHER_HPP_
#define _BATCHER_HPP_

#include <string>

#include "../include/utility.hpp"

namespace Mushroom {

class KeySlice;
class Page;

// batcher is just a wrapper of page, we use it to store keys in order before
// divide them into batch
class Batcher : private NoCopy
{
	public:
		Batcher();

		~Batcher();

		bool InsertKeySlice(const KeySlice *slice);

		uint32_t TotalKey() const;

		uint32_t Capacity() const;

		void BeforeGet();

		const KeySlice* GetKeySlice(uint32_t idx) const;

		std::string ToString() const;

	private:
		Page     *page_;
		uint16_t *idx_;
};

} // Mushroom

#endif /* _BATCHER_HPP_ */