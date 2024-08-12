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
	state_ = State::IN_ROOM;
}
void NetworkClient::leaveRoomEventAction(int playerNr, bool isInactive)
{

}
void NetworkClient::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{

}
void NetworkClient::connectReturn(int errorCode,
	const ExitGames::Common::JString& errorString,
	const ExitGames::Common::JString& region,
	const ExitGames::Common::JString& cluster)
{
	LogDebug("CONNECTED!");
	state_ = State::CONNECTED_TO_MASTER;
}
void NetworkClient::disconnectReturn(void)
{

}
void NetworkClient::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{

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