/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 09:36:52
**/

#ifndef _LOG_HPP_
#define _LOG_HPP_

namespace Mushroom {

#define Info(...)  Log::GetLogger().logv(__VA_ARGS__);
#define Warn(...)  Log::GetLogger().logv(__VA_ARGS__);
#define Error(...) Log::GetLogger().logv(__VA_ARGS__);
#define FatalIf(b, ...) if (b) Log::GetLogger().logv(__VA_ARGS__);

class Log
{
	public:
		static Log& GetLogger();

		void logv(const char *fmt ...);

	private:
		Log();
};

} // namespace Mushroom

#endif /* _LOG_HPP_ */