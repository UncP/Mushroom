/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-30 09:17:51
**/

#ifndef _MARSHALLER_HPP_
#define _MARSHALLER_HPP_

#include <vector>

#include "../network/buffer.hpp"

namespace Mushroom {

class Marshaller
{
	public:
		Marshaller():input_(0), output_(0) { }

		Marshaller(Buffer *input, Buffer *output):input_(input), output_(output) { }

		template<typename T>
		inline void MarshalArgs(uint32_t id, uint32_t rid, const T *args);

		template<typename T>
		inline void MarshalReply(uint32_t rid, const T *reply);

		inline void Read(const void *str, uint32_t len);

		inline void Write(void *str, uint32_t len);

		inline uint32_t HasCompleteArgs();

		inline void Dump(uint32_t size);

	private:
		inline void Unget(uint32_t size);

		Buffer *input_;
		Buffer *output_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const int32_t &v) {
	marshaller.Read(&v, 4);
	return marshaller;
}

inline Marshaller& operator<<(Marshaller &marshaller, const uint32_t &v) {
	marshaller.Read(&v, 4);
	return marshaller;
}

template<typename T>
inline Marshaller& operator<<(Marshaller &marshaller, const std::vector<T> &v) {
	uint32_t e = v.size();
	marshaller << e;
	for (uint32_t i = 0; i < e; ++i)
		marshaller << v[i];
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, int32_t &v) {
	marshaller.Write(&v, 4);
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, uint32_t &v) {
	marshaller.Write(&v, 4);
	return marshaller;
}

template<typename T>
inline Marshaller& operator>>(Marshaller &marshaller, std::vector<T> &v) {
	uint32_t e;
	marshaller >> e;
	v.reserve(e);
	for (uint32_t i = 0; i < e; ++i) {
		T t;
		marshaller >> t;
		v.push_back(t);
	}
	return marshaller;
}

template<typename T>
inline void Marshaller::MarshalArgs(uint32_t id, uint32_t rid, const T *args)
{
	output_->Reset();
	uint32_t *len = (uint32_t *)output_->end();
	output_->AdvanceTail(4);
	uint32_t before = output_->size();
	*this <<  id;
	*this << rid;
	*this << *args;
	*len = output_->size() - before;
}

template<typename T>
inline void Marshaller::MarshalReply(uint32_t rid, const T *reply)
{
	output_->Reset();
	uint32_t *len = (uint32_t *)output_->end();
	output_->AdvanceTail(4);
	uint32_t before = output_->size();
	*this <<  rid;
	*this << *reply;
	*len = output_->size() - before;
}

inline void Marshaller::Read(const void *str, uint32_t len)
{
	output_->Read((const char *)str, len);
}

inline void Marshaller::Write(void *str, uint32_t len)
{
	input_->Write((char *)str, len);
}

inline void Marshaller::Unget(uint32_t size)
{
	input_->Unget(size);
}

inline void Marshaller::Dump(uint32_t size)
{
	input_->AdvanceHead(size);
}

inline uint32_t Marshaller::HasCompleteArgs()
{
	if (input_->size() < 4)
		return 0;
	uint32_t packet_size;
	*this >> packet_size;
	if (input_->size() >= packet_size) {
		return packet_size;
	} else {
		Unget(4);
		return 0;
	}
}

} // namespace Mushroom

#endif /* _MARSHALLER_HPP_ */