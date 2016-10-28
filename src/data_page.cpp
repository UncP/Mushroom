/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 16:02:16
**/

#include <cassert>
#include <unistd.h>

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

page_id DataPage::PutData(const DataSlice *slice)
{
	uint16_t len = slice->Length() + DataSlice::LengthByte;
	if ((curr_ + len) > PageSize)
		return 0;

	page_id res = page_no_;
	res <<= 12;
	res |= curr_ & 0xFFF;

	memcpy(data_ + curr_, slice, len);
	curr_ += len;

	++total_;
	dirty_ = true;

	return res;
}

const DataSlice* DataPage::GetData(page_id page_no) const
{
	assert(page_no_ == (page_no >> 12));
	uint16_t pos = page_no & 0xFFF;
	return (const DataSlice *)(this + pos);
}

Status DataPage::Read(const page_id page_no, const int fd)
{
	if (pread(fd, this, PageSize, page_no * PageSize) != PageSize)
		return Fail;
	assert(page_no_ == page_no);
	return Success;
}

Status DataPage::Write(const int fd)
{
	if (dirty_) {
		dirty_ = 0;
		if (pwrite(fd, this, PageSize, page_no_ * PageSize) != PageSize)
			return Fail;
	}
	return Success;
}

} // namespace Mushroom
