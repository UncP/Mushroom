/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 14:10:19
**/

#ifndef _LOG_MANAGER_HPP_
#define _LOG_MANAGER_HPP_

#include "../include/utility.hpp"
#include "../include/mutex.hpp"

namespace Mushroom {

class Page;

class LogManager : private NoCopy
{
	public:
		LogManager(const char *dir);

		~LogManager();

		Page* NeedRecover();

		bool NeedFlush();

		void Logging(Page *page);

		static void SetManagerInfo(uint32_t log_page);

		static uint32_t LogPage;

	private:
		int      fd_;
		char    *mem_;
		uint32_t cur_page_;
		Mutex    mutex_;
};

} // namespace Mushroom

#endif /* _LOG_MANAGER_HPP_ */