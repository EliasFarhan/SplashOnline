
#include "engine/engine.h"
#include "network/client.h"
#include "graphics/texture_manager.h"

#include <SDL_main.h>

class NetworkTestSystem : public splash::SystemInterface, public splash::DrawInterface
{
public:
	explicit NetworkTestSystem(splash::NetworkClient* client): client_(client)
	{
		splash::AddSystem(this);
		splash::AddDrawInterface(this);
	}
	void Begin() override
	{

	}
	void End() override
	{
		splash::RemoveDrawInterface(this);
		splash::RemoveSystem(this);
	}
	void Update([[maybe_unused]] float dt) override
	{
		if(splash::IsTexturesLoaded() && bg_ == nullptr)
		{
			bg_ = splash::GetTexture(splash::TextureManager::TextureId::BG);
		}
		playerInput_ = splash::GetPlayerInput();
		if(client_->GetState() == splash::NetworkClient::State::IN_GAME)
		{
			splash::InputPacket packet{};
			packet.frame = frame_;
			packet.inputSize = 1;
			packet.playerNumber = client_->GetPlayerIndex()-1;
			packet.inputs[0] = playerInput_;
			client_->SendInputPacket(packet);
			frame_++;
		}
	}
	[[nodiscard]] int GetSystemIndex() const override
	{
		return systemIndex_;
	}
	void SetSystemIndex(int index) override
	{
		systemIndex_ = index;
	}

	void Draw() override
	{
		if(bg_)
		{
			auto* renderer = splash::GetRenderer();
			SDL_RenderCopy(renderer,
				bg_,
				nullptr,
				nullptr);
		}
	}

	void SetGraphicsIndex(int index) override
	{
		graphicsIndex_ = index;
	}

	int GetGraphicsIndex() const override
	{
		return graphicsIndex_;
	}

private:
	splash::NetworkClient* client_ = nullptr;
	SDL_Texture* bg_ = nullptr;
	int systemIndex_ = -1;
	int graphicsIndex_ = -1;
	int frame_ = 0;
	splash::PlayerInput playerInput_{};
};

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{


	ExitGames::LoadBalancing::ClientConstructOptions clientConstructOptions{};
	clientConstructOptions.setRegionSelectionMode(ExitGames::LoadBalancing::RegionSelectionMode::SELECT);
	splash::NetworkClient client{clientConstructOptions};
	NetworkTestSystem networkTestSystem(&client);

	splash::RunEngine();
    return 0;
}