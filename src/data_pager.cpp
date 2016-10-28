/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-25 22:21:31
**/

#include <cassert>

#include "data_pager.hpp"

namespace Mushroom {

DataPager::DataPager(int fd):fd_(fd)
{
	curr_ = (DataPage *)new char[DataPage::PageSize];
	assert(curr_);
	curr_->Reset(0);
}

Status DataPager::PutData(const DataSlice *slice, page_id &page_no)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (!(page_no = curr_->PutData(slice))) {
		curr_->Write(fd_);
		curr_->Reset(curr_->PageNo() + 1);
		// curr_ = DataPage::NewPage(curr_->PageNo() + 1);
	}
	page_no = curr_->PutData(slice);
	return Success;
}

Status DataPager::GetData(const page_id page_no)
{
	assert(0);
	// if (curr_->PageNo() == (page_no >> 12)) {
	// 	curr_->GetData(page_no);
	// 	return Success;
	// }
	// return Success;
}

} // namespace Mushroom
