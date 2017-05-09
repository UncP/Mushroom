/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 09:17:51
**/

#ifndef _MARSHALLER_HPP_
#define _MARSHALLER_HPP_

#include <vector>

#include "../include/utility.hpp"
#include "../include/atomic.hpp"
#include "../network/buffer.hpp"

namespace Mushroom {

class Marshaller : private NoCopy
{
	public:
		static atomic_32_t RpcId;

		Marshaller():input_(0), output_(0) { }

		Marshaller(Buffer *input, Buffer *output):input_(input), output_(output) { }

		template<typename T>
		inline void MarshalArgs(uint32_t id, const T *args);

		template<typename T>
		inline void MarshalReply(uint32_t rid, const T *reply);

		inline void Read(const void *str, uint32_t len);

		inline void Write(void *str, uint32_t len);

		inline bool HasCompleteArgs();

	private:
		inline void Unget(uint32_t size);

		bool    own_buf_;
		Buffer *input_;
		Buffer *output_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const uint32_t &v) {
	marshaller.Read(&v, 4);
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, uint32_t &v) {
	marshaller.Write(&v, 4);
	return marshaller;
}

template<typename T>
inline void Marshaller::MarshalArgs(uint32_t id, const T *args)
{
	uint32_t rid = RpcId++;
	output_->Reset();
	uint32_t *len = (uint32_t *)output_->begin();
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
	uint32_t *len = (uint32_t *)output_->begin();
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

inline uint32_t Marshaller::HasCompleteArgs()
{
	if (input_->size() < 4)
		return false;
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