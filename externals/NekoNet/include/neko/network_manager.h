

#ifndef NEKO_NETWORK_MANAGER_H
#define NEKO_NETWORK_MANAGER_H

#include <LoadBalancing-cpp/inc/Client.h>

namespace neko
{

class ClientInterface : public ExitGames::LoadBalancing::Listener
{
};

class NetworkManager
{
public:

    explicit NetworkManager(ClientInterface* client);
	void Begin();
	void Tick();
	void End();

	ExitGames::LoadBalancing::Client& GetClient();
private:
    ExitGames::LoadBalancing::Client loadBalancingClient_;
};
ExitGames::LoadBalancing::Client& GetLoadBalancingClient();

} // namespace neko



#endif //NEKO_NETWORK_MANAGER_H