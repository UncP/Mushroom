/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-19 13:09:33
**/

#ifndef _PAGE_MANAGER_HPP_
#define _PAGE_MANAGER_HPP_

#include "btree_page.hpp"

namespace Mushroom {

class PageManager
{
	public:
		PageManager(int fd, page_id *cur);

		BTreePage* GetPage(page_id page_no);
		BTreePage* NewPage(int type, uint8_t key_len, uint8_t level, uint16_t degree);

		bool Free();

		page_id Total() const { return *cur_; }

		PageManager(const PageManager &) = delete;
		PageManager(const PageManager &&) = delete;
		PageManager& operator=(const PageManager &) = delete;
		PageManager& operator=(const PageManager &&) = delete;

		static const page_id LatchPages = 3;

	private:
		int   fd_;
		volatile page_id *cur_;
		char *mem_;
};

} // namespace Mushroom

#endif /* _PAGE_MANAGER_HPP_ */