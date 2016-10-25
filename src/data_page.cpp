/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 16:02:16
**/

#include "data_page.hpp"

namespace Mushroom {

void DataPage::Reset(page_id page_no)
{
	page_no_ = page_no;
	total_   = 0;
	curr_    = data_ - (char *)this;
	dirty_   = 0;
}

DataPage* DataPage::NewPage(page_id page_no)
{
	DataPage *page = (DataPage *)new char[PageSize];
	if (!page) return page;
	page->Reset(page_no);
	return page;
}

page_id DataPage::GetSlot(uint16_t data_len) const
{
	if ((curr_ + data_len) > PageSize)
		return 0;
	page_id res = page_no;
	res <<= 12;
	res |= curr_ & 0xFFF;
	return res;
}

void DataPage::PutDataSlice(uint16_t pos, const DataSlice *slice)
{
	memcpy(data_ + pos, slice, slice->Length() + 2);
	curr_ += slice->Length();
}

} // namespace Mushroom
