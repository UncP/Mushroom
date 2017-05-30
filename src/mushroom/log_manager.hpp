/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-05-29 14:10:19
**/

#ifndef _LOG_MANAGER_HPP_
#define _LOG_MANAGER_HPP_

#include "../include/utility.hpp"

namespace Mushroom {

class Page;

class LogManager : private NoCopy
{
	public:
		LogManager(const char *dir);

		~LogManager();

		void Logging(Page *page);

	private:
		int   fd_;
		char *mem_;
};

} // namespace Mushroom

#endif /* _LOG_MANAGER_HPP_ */