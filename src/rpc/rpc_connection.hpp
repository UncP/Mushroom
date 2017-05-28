/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-02 21:22:07
**/

#ifndef _RPC_CONNECTION_HPP_
#define _RPC_CONNECTION_HPP_

#include <map>
// #include <random>
// #include <ctime>

#include "../include/atomic.hpp"
#include "../include/mutex.hpp"
#include "../network/connection.hpp"
#include "rpc.hpp"
#include "marshaller.hpp"
#include "future.hpp"

// static std::default_random_engine engine(time(0));
// static std::uniform_real_distribution<float> dist(0, 1);

namespace Mushroom {

class RpcConnection : public Connection
{
	public:
		static atomic_32_t RpcId;

		RpcConnection(const EndPoint &server, Poller *poller, float error_rate);

		RpcConnection(const Socket &socket, Poller *poller);

		~RpcConnection();

		template<typename T1, typename T2>
		inline void Call(const char *str, const T1 *args, Future<T2> *fu);

		template<typename T>
		inline void RemoveFuture(Future<T> *fu);

		bool Close();

		Marshaller& GetMarshaller();

		bool Disabled();

		void Disable();

		void Enable();

		using Connection::OnRead;

	private:
		using Connection::Send;
		using Connection::OnWrite;

		atomic_32_t disable_;
		float       error_rate_;

		Mutex                    mutex_;
		std::map<uint32_t, Func> futures_;

		Marshaller marshaller_;
};

template<typename T1, typename T2>
inline void RpcConnection::Call(const char *str, const T1 *args, Future<T2> *fu)
{
	uint32_t id  = RPC::Hash(str);
	uint32_t rid = RpcId++;
	fu->SetId(rid);
	mutex_.Lock();
	futures_.insert({rid, std::move([fu, this]() { fu->Notify(marshaller_); })});
	if (!disable_.get()) { // && dist(engine) > error_rate_
		marshaller_.MarshalArgs(id, rid, args);
		SendOutput();
	}
	mutex_.Unlock();
}

template<typename T>
inline void RpcConnection::RemoveFuture(Future<T> *fu)
{
	mutex_.Lock();
	auto it = futures_.find(fu->GetId());
	if (it != futures_.end())
		futures_.erase(it);
	mutex_.Unlock();
}

} // namespace Mushroom

#endif /* _RPC_CONNECTION_HPP_ */