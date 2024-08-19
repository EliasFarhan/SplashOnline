
#include "engine/engine.h"
#include "network/client.h"

#include <SDL_main.h>

class NetworkTestSystem : public splash::SystemInterface
{
public:
	explicit NetworkTestSystem(splash::NetworkClient* client): client_(client)
	{
		splash::AddSystem(this);
	}
	void Begin() override
	{

	}
	void End() override
	{
		splash::RemoveSystem(this);
	}
	void Update(float dt) override
	{
		playerInput_ = splash::GetPlayerInput();
		if(client_->GetState() == splash::NetworkClient::State::IN_GAME)
		{
			splash::InputPacket packet{};
			packet.frame = frame_;
			packet.inputSize = 1;
			packet.inputs[0] = playerInput_;
			client_->SendInputPacket(packet);
			frame_++;
		}
	}
	int GetSystemIndex() const override
	{
		return systemIndex_;
	}
	void SetSystemIndex(int index) override
	{
		systemIndex_ = index;
	}
private:
	splash::NetworkClient* client_ = nullptr;
	int systemIndex_ = -1;
	splash::PlayerInput playerInput_{};
	int frame_ = 0;
};

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{

	splash::Engine engine;

	ExitGames::LoadBalancing::ClientConstructOptions clientConstructOptions{};
	clientConstructOptions.setRegionSelectionMode(ExitGames::LoadBalancing::RegionSelectionMode::SELECT);
	splash::NetworkClient client{clientConstructOptions};
	NetworkTestSystem networkTestSystem(&client);

	engine.Run();
    return 0;
}