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

		static DataPage* NewPage(page_id page_no);

		page_id GetSlot(uint16_t data_len) const;

		void Reset(page_id page_no);

		void PutDataSlice()

	private:
		page_id   page_no_;
		uint16_t  total_;
		uint16_t  curr_;
		uint8_t   dirty_;
		char      data_[0];
};

} // namespace Mushroom

#endif /* _DATA_PAGE_HPP_ */