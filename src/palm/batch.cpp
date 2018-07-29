/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-28 15:36:00
**/

#include "batch.hpp"
#include "../blink/slice.hpp"

namespace Mushroom {

uint32_t Batch::Size = 64;

void Batch::SetSize(uint32_t size)
{
	Size = size;
}

Batch::Batch()
{
	batch_ = new KeySlice*[Size];
	for (uint32_t i = 0; i < Size; ++i)
		batch_[i] = NewKeySlice();
}

Batch::~Batch()
{
	for (int32_t i = (int32_t)Size - 1; i >= 0; --i)
		DeleteKeySlice(batch_[i]);
	delete [] batch_;
}

// not check boundary for performance reason
void Batch::SetKeySlice(uint32_t idx, const char *key)
{
	memcpy(batch_[idx]->key_, key, KeySlice::KeyLen);
}

// not check boundary for performance reason
const KeySlice* Batch::GetKeySlice(uint32_t idx) const
{
	return batch_[idx];
}

} // Mushroom
