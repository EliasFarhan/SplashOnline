

#ifndef NEKO_NETWORK_MANAGER_H
#define NEKO_NETWORK_MANAGER_H


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#pragma clang diagnostic ignored "-Wold-style-cast"
#include <LoadBalancing-cpp/inc/Client.h>
#pragma clang diagnostic pop

namespace neko
{

class ClientInterface : public ExitGames::LoadBalancing::Listener
{
};

class NetworkManager
{
public:

    explicit NetworkManager(ClientInterface* client, const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions={});
	virtual ~NetworkManager() = default;
	virtual void Begin();
	virtual void Tick();
	virtual void End();

	ExitGames::LoadBalancing::Client& GetClient();
protected:
    ExitGames::LoadBalancing::Client loadBalancingClient_;
	ExitGames::LoadBalancing::ConnectOptions connectOptions_;
};
ExitGames::LoadBalancing::Client& GetLoadBalancingClient();

} // namespace neko



#endif //NEKO_NETWORK_MANAGER_H