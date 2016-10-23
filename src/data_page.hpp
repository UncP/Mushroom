/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-10 16:00:55
**/

#ifndef _DATA_PAGE_HPP_
#define _DATA_PAGE_HPP_

#include "slice.hpp"

namespace Mushroom {

class DataPage
{
	public:
		static const uint16_t PageSize = 4096;

	private:
		page_id   page_no_;
		uint16_t  total_;
		DataSlice data_[0];
};

class DataPager
{
	public:

};

} // namespace Mushroom

#endif /* _DATA_PAGE_HPP_ */