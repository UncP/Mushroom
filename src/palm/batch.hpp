/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-28 15:33:00
**/

#ifndef _BATCH_HPP_
#define _BATCH_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class KeySlice;

class Batch : private NoCopy
{
	public:
		static uint32_t Size;

		static void SetSize(uint32_t size);

		Batch();

		~Batch();

		void SetKeySlice(uint32_t idx, const char *key);

		const KeySlice* GetKeySlice(uint32_t idx) const;

	private:
		KeySlice **batch_;
};

} // Mushroom

#endif /* _BATCH_HPP_ */