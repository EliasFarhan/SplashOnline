//
// Created by unite on 19.07.2024.
//
#include "utils/log.h"
#include "engine/engine.h"
#include <neko/network_manager.h>

#include <SDL_main.h>
#include <fmt/format.h>

#include <imgui.h>

namespace splash
{
class ClientTest : public neko::ClientInterface, public OnGuiInterface, public SystemInterface
{
public:

	enum class State
	{
		CONNECTING,
		CONNECTED_TO_MASTER,
		JOINING,
		IN_ROOM,
		IN_GAME,
	};

	ClientTest()
	{
		AddGuiInterface(this);
		AddSystem(this);
	}

	void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override
	{
		LogDebug(fmt::format("Debug Return: {} with msg: {}", debugLevel, string.ASCIIRepresentation().cstr()));
	}
	void connectionErrorReturn(int errorCode) override
	{
		LogError(fmt::format("Connection Error, code: {}", errorCode));
	}
	void clientErrorReturn(int errorCode) override
	{
		LogError(fmt::format("Client Error, code: {}", errorCode));
	}
	void warningReturn(int warningCode) override
	{
		LogWarning(fmt::format("Warning, code: {}", warningCode));
	}
	void serverErrorReturn(int errorCode) override
	{
		LogError(fmt::format("Server Error, code: {}", errorCode));
	}
	void joinRoomEventAction(int playerNr,
		const ExitGames::Common::JVector<int>& playernrs,
		const ExitGames::LoadBalancing::Player& player) override
	{

	}
	void leaveRoomEventAction(int playerNr, bool isInactive) override
	{

	}
	void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override
	{

	}
	void connectReturn(int errorCode,
		const ExitGames::Common::JString& errorString,
		const ExitGames::Common::JString& region,
		const ExitGames::Common::JString& cluster) override
	{
		LogDebug("CONNECTED!");
		state_ = State::CONNECTED_TO_MASTER;
	}
	void disconnectReturn(void) override
	{
		LogDebug("Disconnect");
	}
	void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override
	{

	}
	void OnGui() override
	{
		ImGui::Begin("Network Client");
		switch(state_)
		{

		case State::CONNECTING:
		{
			ImGui::Text("Connecting...");
			break;
		}
		case State::CONNECTED_TO_MASTER:
		{
			if(ImGui::Button("Join Or Create"))
			{
				state_ = State::JOINING;
				auto& client = neko::GetLoadBalancingClient();
				client.opJoinRandomOrCreateRoom();
			}
			break;
		}
		case State::JOINING:
		{
			ImGui::Text("Joining...");
			break;
		}
		case State::IN_ROOM:
		{
			break;
		}
		case State::IN_GAME:
		{
			break;
		}
		default:
			break;
		}
		ImGui::End();
	}
	void SetGuiIndex(int index) override
	{
		guiIndex_ = index;
	}
	[[nodiscard]] int GetGuiIndex() const override
	{
		return guiIndex_;
	}
	void Begin() override
	{
	}
	void End() override
	{
		RemoveSystem(this);
		RemoveGuiInterface(this);
	}
	void Update([[maybe_unused]] float dt) override
	{

	}
	[[nodiscard]] int GetSystemIndex() const override
	{
		return systemIndex_;
	}
	void SetSystemIndex(int index) override
	{
		systemIndex_ = index;
	}
private:
	State state_ = State::CONNECTING;
	int guiIndex_ = -1;
	int systemIndex_ = -1;
};

class NetworkManager : public SystemInterface
{
public:
	NetworkManager(neko::ClientInterface* clientInterface): networkManager(clientInterface)
	{
		AddSystem(this);
	}

	void Begin() override
	{
		networkManager.Begin();
	}

	void End() override
	{
		networkManager.End();
	}

	void Update(float dt) override
	{
		networkManager.Tick();
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
	neko::NetworkManager networkManager;
	int systemIndex_ = -1;
};

}

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{

	splash::Engine engine;
	splash::ClientTest client;
	splash::NetworkManager networkManager(&client);

	engine.Run();
    return 0;
}