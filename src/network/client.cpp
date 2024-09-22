#include "network/client.h"
#include "engine/engine.h"
#include "utils/log.h"
#include "network/packet.h"

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

	if(state_.load(std::memory_order_consume) == State::JOINING)
	{
		state_.store(State::IN_ROOM, std::memory_order_release);
		localPlayerIndex_ = networkManager_.GetClient().getLocalPlayer().getNumber();
		const auto& room = networkManager_.GetClient().getCurrentlyJoinedRoom();
		isMaster_ = room.getMasterClientID() == localPlayerIndex_;
	}
}
void NetworkClient::leaveRoomEventAction(int playerNr, bool isInactive)
{
	(void) isInactive;
	(void) playerNr;
	auto& room = networkManager_.GetClient().getCurrentlyJoinedRoom();
	isMaster_ = room.getMasterClientID() == localPlayerIndex_;
	if(state_.load(std::memory_order_consume) == State::IN_GAME)
	{
		state_.store(State::IN_ROOM);
		room.setIsOpen(true);
	}
}
void NetworkClient::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	(void) playerNr;
	(void) eventContent;
	switch(static_cast<PacketType>(eventCode))
	{

	case PacketType::START_GAME:
	{
		if(state_.load(std::memory_order_consume) == State::IN_ROOM)
		{
			state_.store(State::IN_GAME, std::memory_order_release);
			LogDebug("In Game");
		}
		break;
	}
	case PacketType::INPUT:
	{
		if(state_.load(std::memory_order_consume) == State::IN_GAME)
		{
			auto lastReceiveInput_ = ExitGames::Common::ValueObject<InputSerializer>(eventContent).getDataCopy();
			std::scoped_lock<std::mutex> lock(inputMutex_);
			lastReceivedInputPackets_.push_back(lastReceiveInput_.GetPlayerInput());
		}
		break;
	}
	case PacketType::CONFIRM_FRAME:
	{
		if(state_.load(std::memory_order_consume) == State::IN_GAME)
		{
			auto lastReceiveConfirm_ = ExitGames::Common::ValueObject<ConfirmFrameSerializer>(eventContent).getDataCopy();
			std::scoped_lock<std::mutex> lock(confirmPacketMutex_);
			lastReceivedConfirmPackets_.push_back(lastReceiveConfirm_.GetConfirmPacket());
		}
		break;
	}
	default:
	{
		LogWarning(fmt::format("Received event packet with unknown code: {}", eventCode));
		break;
	}
	}
}
void NetworkClient::connectReturn(int errorCode,
	const ExitGames::Common::JString& errorString,
	const ExitGames::Common::JString& region,
	const ExitGames::Common::JString& cluster)
{
	(void) errorCode;
	(void) errorString;
	LogDebug(fmt::format("Connect Return: region: {} cluster: {}", region.ASCIIRepresentation().cstr(), cluster.ASCIIRepresentation().cstr()));
	state_.store(State::CONNECTED_TO_MASTER, std::memory_order_release);
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
	InputSerializer::registerType();
	ConfirmFrameSerializer::registerType();
}
void NetworkClient::End()
{
	ConfirmFrameSerializer::unregisterType();
	InputSerializer::unregisterType();
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
	const auto state = state_.load(std::memory_order_consume);
	if(state == State::IN_GAME)
	{
		return;
	}
	ImGui::Begin("Network Client");
	switch(state_.load(std::memory_order_consume))
	{
	case State::UNCONNECTED:
	{
		if(ImGui::Button("Connect"))
		{
			ScheduleNetJob(&networkJob_);
			state_.store(State::CONNECTING, std::memory_order_release);
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
			state_.store(State::JOINING, std::memory_order_release);
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
	case State::CHOOSING_REGIONS:
	{
		ImGui::Text("Choose region:");
		for(const auto& region : regions_)
		{
			if(ImGui::Button(region.first.c_str()))
			{
				std::scoped_lock<std::mutex> lock(networkTasksMutex_);
				networkTasks_.emplace_back([region, this]{
					auto& client = networkManager_.GetClient();
					client.selectRegion(region.first.c_str());
				});
				break;

			}
		}
		break;
	}
	case State::IN_ROOM:
	{
		ImGui::Text("In Room");
		auto& client = neko::GetLoadBalancingClient();
		if (!client.getIsInRoom())
			break;
		auto& room = client.getCurrentlyJoinedRoom();
		const auto playerCount = room.getPlayerCount();
		ImGui::Text("Player Count: %d", playerCount);
		ImGui::Text("Player Nbr: %d", localPlayerIndex_);
		if(playerCount >= 2 && isMaster_)
		{
			if(ImGui::Button("Start Game"))
			{
				std::scoped_lock<std::mutex> lock(networkTasksMutex_);
				networkTasks_.emplace_back([this]{
					state_.store(State::IN_GAME, std::memory_order_release);

					auto& client = neko::GetLoadBalancingClient();
					auto& room = client.getCurrentlyJoinedRoom();
					room.setIsOpen(false);
					ExitGames::LoadBalancing::RaiseEventOptions options{};
					client.opRaiseEvent(true, static_cast<nByte>(0), static_cast<nByte>(PacketType::START_GAME), options);
				});
			}
		}

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
NetworkClient::NetworkClient(const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions) : networkManager_(this, clientConstructOptions), networkJob_([this](){RunNetwork();})
{
	instance = this;
	AddGuiInterface(this);
	AddSystem(this);
	networkTasks_.reserve(15);
	returnedInputPackets_.reserve(10);
	lastReceivedInputPackets_.reserve(10);
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
	static constexpr Uint32 tickTime = 10;//10ms for network loop
	auto previous = SDL_GetTicks();
	while(isRunning_.load(std::memory_order_consume))
	{

		{
#ifdef TRACY_ENABLE
			ZoneNamedN(networkTasks, "Network Tasks", true);
#endif
			std::scoped_lock<std::mutex> lock(networkTasksMutex_);
			for(auto& networkTask: networkTasks_)
			{
				networkTask();
			}
			networkTasks_.clear();
		}
#ifdef TRACY_ENABLE
		TracyCZoneN(networkLoop, "Network Loop", true);
#endif
		networkManager_.Tick();
#ifdef TRACY_ENABLE
		TracyCZoneEnd(networkLoop);
#endif
		auto current = SDL_GetTicks();
		if(current - previous < tickTime)
		{
			SLEEP(tickTime - (current-previous));
		}
		previous = current;
	}
#ifdef TRACY_ENABLE
	TracyCZoneN(netEnd, "Stop Network Manager", true);
#endif
	networkManager_.End();
#ifdef TRACY_ENABLE
	TracyCZoneEnd(netEnd);
#endif
}
int NetworkClient::GetPlayerIndex() const
{
	return localPlayerIndex_;
}
void NetworkClient::SendInputPacket(const InputPacket& inputPacket)
{
	InputSerializer serializer(inputPacket);
	std::scoped_lock<std::mutex> lock(networkTasksMutex_);
	networkTasks_.emplace_back([this, serializer]
	{
		auto& client = networkManager_.GetClient();
		ExitGames::LoadBalancing::RaiseEventOptions options{};
		client.opRaiseEvent(false, serializer, static_cast<nByte>(PacketType::INPUT), options);
	});
}
void NetworkClient::SendConfirmFramePacket(const ConfirmFramePacket& confirmPacket)
{
	ConfirmFrameSerializer serializer(confirmPacket);
	std::scoped_lock<std::mutex> lock(networkTasksMutex_);
	networkTasks_.emplace_back([this, serializer]
	{
		auto& client = networkManager_.GetClient();
		ExitGames::LoadBalancing::RaiseEventOptions options{};
		client.opRaiseEvent(true, serializer, static_cast<nByte>(PacketType::CONFIRM_FRAME), options);
	});
}
void NetworkClient::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& regions,
	const ExitGames::Common::JVector<ExitGames::Common::JString>& regionsServers)
{
	state_.store(State::CHOOSING_REGIONS);
	std::vector<std::pair<std::string, std::string>> regionsTmp;
	regionsTmp.reserve(regions.getSize());
	for(unsigned i = 0; i < regions.getSize(); i++)
	{
		regionsTmp.emplace_back(regions[i].ASCIIRepresentation().cstr(), regionsServers[i].ASCIIRepresentation().cstr());
	}
	std::swap(regions_, regionsTmp);
}
neko::Span<InputPacket> NetworkClient::GetInputPackets()
{
	{
		std::scoped_lock<std::mutex> lock(inputMutex_);
		std::swap(lastReceivedInputPackets_, returnedInputPackets_);
		lastReceivedInputPackets_.clear();
	}
	return {returnedInputPackets_.data(), returnedInputPackets_.size()};
}
std::array<bool, MaxPlayerNmb> NetworkClient::GetConnectedPlayers()
{
	std::array<bool, MaxPlayerNmb> connectedPlayers{};
	const auto& room = networkManager_.GetClient().getCurrentlyJoinedRoom();
	const auto& players = room.getPlayers();
	for(unsigned i = 0; i < players.getSize(); i++)
	{
		const auto* playerTmp = players[i];
		connectedPlayers[playerTmp->getNumber()-1] = true;
	}
	return connectedPlayers;
}
neko::Span<ConfirmFramePacket> NetworkClient::GetConfirmPackets()
{
	{
		std::scoped_lock<std::mutex> lock(confirmPacketMutex_);
		std::swap(lastReceivedConfirmPackets_, returnedConfirmPackets_);
		lastReceivedConfirmPackets_.clear();
	}
	return {returnedConfirmPackets_.data(), returnedConfirmPackets_.size()};
}
NetworkClient* GetNetworkClient()
{
	return instance;
}
}