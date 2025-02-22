

#ifndef NEKO_NETWORK_MANAGER_H
#define NEKO_NETWORK_MANAGER_H

#include <LoadBalancing-cpp/inc/Client.h>

namespace neko
{

class ClientInterface : public ExitGames::LoadBalancing::Listener
{
};

namespace NetworkManager
{
void Begin(ClientInterface* client, const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions={});
void Tick();
void End();
};
ExitGames::LoadBalancing::Client& GetLoadBalancingClient();

} // namespace neko



#endif //NEKO_NETWORK_MANAGER_H