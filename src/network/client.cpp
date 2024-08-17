#include "network/client.h"
#include "engine/engine.h"
#include "utils/log.h"

#include <imgui.h>
#include <fmt/format.h>
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif


namespace splash
{

static NetworkClient* instance = nullptr;
void NetworkClient::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
{
	LogDebug(fmt::format("Debug Return: {} with msg: {}", debugLevel, string.ASCIIRepresentation().cstr()));
}
void NetworkClient::connectionErrorReturn(int errorCode)
{
	LogError(fmt::format("Connection Error, code: {}", errorCode));
}
void NetworkClient::clientErrorReturn(int errorCode)
{
	LogError(fmt::format("Client Error, code: {}", errorCode));
}
void NetworkClient::warningReturn(int warningCode)
{
	LogWarning(fmt::format("Warning, code: {}", warningCode));
}
void NetworkClient::serverErrorReturn(int errorCode)
{
	LogError(fmt::format("Server Error, code: {}", errorCode));
}
void NetworkClient::joinRoomEventAction(int playerNr,
	const ExitGames::Common::JVector<int>& playernrs,
	const ExitGames::LoadBalancing::Player& player)
{
	(void) player;
	(void) playerNr;
	(void) playernrs;

	LogDebug(fmt::format("Join Room Event: playerNr: {} player name: {}", playerNr, player.getName().ASCIIRepresentation().cstr()));

	state_ = State::IN_ROOM;
}
void NetworkClient::leaveRoomEventAction(int playerNr, bool isInactive)
{
	(void) playerNr;
	(void) isInactive;
}
void NetworkClient::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	(void) playerNr;
	(void) eventCode;
	(void) eventContent;
}
void NetworkClient::connectReturn(int errorCode,
	const ExitGames::Common::JString& errorString,
	const ExitGames::Common::JString& region,
	const ExitGames::Common::JString& cluster)
{
	(void) errorCode;
	(void) errorString;
	LogDebug(fmt::format("Connect Return: region: {} cluster: {}", region.ASCIIRepresentation().cstr(), cluster.ASCIIRepresentation().cstr()));
	state_ = State::CONNECTED_TO_MASTER;
}
void NetworkClient::disconnectReturn(void)
{
	LogDebug("Disconnect Return");
}
void NetworkClient::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	(void)errorCode;
	(void)errorString;
}
void NetworkClient::Begin()
{
}
void NetworkClient::End()
{
	RemoveSystem(this);
	RemoveGuiInterface(this);
	isRunning_.store(false, std::memory_order_release);
}
void NetworkClient::Update(float dt)
{
	(void) dt;
}
int NetworkClient::GetSystemIndex() const
{
	return systemIndex_;
}
void NetworkClient::SetSystemIndex(int index)
{
	systemIndex_ = index;
}
void NetworkClient::OnGui()
{
	ImGui::Begin("Network Client");
	switch(state_)
	{
	case State::UNCONNECTED:
	{
		if(ImGui::Button("Connect"))
		{
			ScheduleNetJob(&networkJob_);
			state_ = State::CONNECTING;
		}
		break;
	}
	case State::CONNECTING:
	{
		ImGui::Text("Connecting...");
		break;
	}
	case State::CONNECTED_TO_MASTER:
	{
		static std::array<char, 40> roomName{};
		static std::array<char, 40> playerName{};
		ImGui::InputText("Player Name", playerName.data(), playerName.size());
		ImGui::InputText("Room Name", roomName.data(), roomName.size());
		if(ImGui::Button("Join Or Create"))
		{
			state_ = State::JOINING;
			ExitGames::LoadBalancing::RoomOptions roomOptions{};
			roomOptions.setMaxPlayers(4);
			auto& client = neko::GetLoadBalancingClient();
			auto& player = client.getLocalPlayer();
			player.setName(playerName.data());
			client.opJoinRandomOrCreateRoom(ExitGames::Common::JString(roomName.data()), roomOptions);
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
		ImGui::Text("In Room");
		auto& client = neko::GetLoadBalancingClient();
		if (!client.getIsInRoom())
			break;
		auto& player1 = client.getLocalPlayer();
		auto& room = client.getCurrentlyJoinedRoom();
		const auto playerCount = room.getPlayerCount();
		ImGui::Text("Player Count: %d", room.getPlayerCount());
		ImGui::Text("Player Nbr: %d", player1.getNumber());
		if(playerCount >= 2)
		{
			if(ImGui::Button("Start Game"))
			{
				state_ = State::IN_GAME;
				//TODO send event start game
				//create the game manager?
			}
		}

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
void NetworkClient::SetGuiIndex(int index)
{
	guiIndex_ = index;
}
int NetworkClient::GetGuiIndex() const
{
	return guiIndex_;
}
NetworkClient::NetworkClient() : networkManager_(this), networkJob_([this](){RunNetwork();})
{
	instance = this;
	AddGuiInterface(this);
	AddSystem(this);
}
void NetworkClient::RunNetwork()
{
#ifdef TRACY_ENABLE
	TracyCZoneN(netStart, "Start Network Manager", true);
#endif
	networkManager_.Begin();
#ifdef TRACY_ENABLE
	TracyCZoneEnd(netStart);
#endif
	while(isRunning_.load(std::memory_order_consume))
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(networkLoop, "Network Loop", true);
#endif
		networkManager_.Tick();
	}
#ifdef TRACY_ENABLE
	TracyCZoneN(netEnd, "Stop Network Manager", true);
#endif
	networkManager_.End();
#ifdef TRACY_ENABLE
	TracyCZoneEnd(netEnd);
#endif
}
NetworkClient* GetNetworkClient()
{
	return instance;
}
}