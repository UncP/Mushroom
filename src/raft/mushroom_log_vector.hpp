/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-14 15:09:51
**/

#ifndef _MUSHROOM_VECTOR_HPP_
#define _MUSHROOM_VECTOR_HPP_

#include "../include/utility.hpp"
#include "../rpc/marshaller.hpp"
#include "mushroom_log.hpp"

namespace Mushroom {

class Buffer;
class MushroomLog;

class LogVector : private NoCopy
{
	public:
		uint32_t size() const;

		const MushroomLog& operator[](uint32_t idx) const;

	private:
		uint32_t size_;
		char     mem_[0];
};

class MushroomLogVector : private NoCopy
{
	public:
		MushroomLogVector();

		~MushroomLogVector();

		uint32_t size() const;

		uint32_t cap() const;

		MushroomLog& operator[](uint32_t idx) const;

		void Append(const MushroomLog &log);

		void Append(const LogVector &that, uint32_t idx);

		void DeleteFrom(uint32_t idx);

		void FormLogVectorFrom(uint32_t idx, Buffer &buf) const;

	private:
		uint32_t  size_;
		uint32_t  cap_;
		char     *mem_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_VECTOR_HPP_ */