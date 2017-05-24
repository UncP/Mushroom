/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-02 21:22:07
**/

#ifndef _RPC_CONNECTION_HPP_
#define _RPC_CONNECTION_HPP_

#include <map>
#include <random>
#include <ctime>

#include "../include/atomic.hpp"
#include "../include/spin_lock.hpp"
#include "../network/connection.hpp"
#include "rpc.hpp"
#include "marshaller.hpp"
#include "future.hpp"

static std::default_random_engine engine(time(0));
static std::uniform_real_distribution<float> dist(0, 1);

namespace Mushroom {

class RpcConnection : public Connection
{
	public:
		static atomic_32_t RpcId;

		RpcConnection(const EndPoint &server, Poller *poller, float error_rate = 0.f);

		RpcConnection(const Socket &socket, Poller *poller);

		~RpcConnection();

		template<typename T1, typename T2>
		inline void Call(const char *str, const T1 *args, Future<T2> *fu);

		Marshaller& GetMarshaller();

		using Connection::OnRead;

		void Disable();

		void Enable();

		template<typename T>
		inline void RemoveFuture(Future<T> *fu);

	private:
		using Connection::Send;
		using Connection::OnWrite;

		atomic_8_t  disable_ = 0;
		float       error_rate_ = 0.f;

		SpinLock                 spin_;
		std::map<uint32_t, Func> futures_;

		Marshaller marshaller_;
};

template<typename T1, typename T2>
inline void RpcConnection::Call(const char *str, const T1 *args, Future<T2> *fu)
{
	uint32_t id  = RPC::Hash(str);
	uint32_t rid = RpcId++;
	spin_.Lock();
	fu->SetId(id);
	futures_[rid] = std::move([fu, this]() { fu->Notify(marshaller_); });
	spin_.Unlock();
	if (!disable_.get() && dist(engine) > error_rate_) {
		marshaller_.MarshalArgs(id, rid, args);
		SendOutput();
	}
}

template<typename T>
inline void RpcConnection::RemoveFuture(Future<T> *fu)
{
	spin_.Lock();
	auto it = futures_.find(fu->Id());
	if (it != futures_.end())
		futures_.erase(it);
	spin_.Unlock();
}

} // namespace Mushroom

#endif /* _RPC_CONNECTION_HPP_ */