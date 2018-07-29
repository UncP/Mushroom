/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2018-7-28 16:46:00
**/

#include <new>

#include "batcher.hpp"
#include "../blink/slice.hpp"
#include "../blink/page.hpp"

namespace Mushroom {

Batcher::Batcher():idx_(0)
{
	char *tmp = new char[Page::PageSize];
	uint16_t degree = Page::CalculateDegree(KeySlice::KeyLen);
	page_ = new (tmp) Page(0, 0, KeySlice::KeyLen, 0, degree);
	page_->InsertInfiniteKey();
}

Batcher::~Batcher()
{
	idx_ = 0;
	delete [] (char *)page_;
}

bool Batcher::InsertKeySlice(const KeySlice *slice)
{
	if (page_->Full()) return false;
	page_t page_no;
	return page_->Insert(slice, page_no) == InsertOk;
}

uint32_t Batcher::TotalKey() const {
	return page_->TotalKey();
}

uint32_t Batcher::Capacity() const {
	return page_->Degree();
}

void Batcher::BeforeGet() {
	idx_ = page_->Index();
}

// before any Get call, must call BeforeGet
const KeySlice* Batcher::GetKeySlice(uint32_t idx) const {
	return page_->Key(idx_, idx);
}

std::string Batcher::ToString() const {
	return page_->ToString(true, true);
}

} // Mushroom
