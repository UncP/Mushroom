/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-25 22:21:21
**/

#ifndef _DATA_PAGER_HPP_
#define _DATA_PAGER_HPP_

#include <mutex>

#include "data_page.hpp"

namespace Mushroom {

class DataPager
{
	public:

		DataPager(int fd);

		Status PutData(const DataSlice *slice, page_id &page_no);

		Status GetData(const page_id page_no);

	private:

		int       fd_;

		std::mutex mutex_;

		DataPage *curr_;
};

} // namespace Mushroom

#endif /* _DATA_PAGER_HPP_ */