/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-29 09:36:52
**/

#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <cassert>
#include <cerrno>
#include <cstring>

namespace Mushroom {

#define Info(...)  Logv::GetLogger().logv(__VA_ARGS__)
#define Warn(...)  Logv::GetLogger().logv(__VA_ARGS__)
#define Error(...) Logv::GetLogger().logv(__VA_ARGS__)
#define Fatal(...) Logv::GetLogger().logv(__VA_ARGS__)
#define ExitIf(b, ...) do { if (b) {Logv::GetLogger().logv(__VA_ARGS__); exit(0);} } while (0)
#define FatalIf(b, ...) do { if (b) {Logv::GetLogger().logv(__VA_ARGS__); assert(0);} } while (0)

class Logv
{
	public:
		static Logv& GetLogger();

		void logv(const char *fmt ...);

	private:
		Logv();
};

} // namespace Mushroom

#endif /* _LOG_HPP_ */