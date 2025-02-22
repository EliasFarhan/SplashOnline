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

#include <memory>


namespace splash
{

class NetworkClientImpl : public neko::ClientInterface
{
public:
	void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override;
	void connectionErrorReturn(int errorCode) override;
	void clientErrorReturn(int errorCode) override;
	void warningReturn(int warningCode) override;
	void serverErrorReturn(int errorCode) override;
	void joinRoomEventAction(int playerNr,
		const ExitGames::Common::JVector<int>& playernrs,
		const ExitGames::LoadBalancing::Player& player) override;
	void leaveRoomEventAction(int playerNr, bool isInactive) override;
	void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;
	void connectReturn(int errorCode,
		const ExitGames::Common::JString& errorString,
		const ExitGames::Common::JString& region,
		const ExitGames::Common::JString& cluster) override;
	void disconnectReturn(void) override;
	void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override;
	void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString> & regions,
		const ExitGames::Common::JVector<ExitGames::Common::JString> & regionsServers) override;

};
class NetworkClientUi : public OnGuiInterface
{
public:
	void OnGui() override;
	void SetGuiIndex(int index) override;
	int GetGuiIndex() const override;
private:
	int guiIndex_ = -1;
};
class NetworkSystem : public SystemInterface
{
public:
	void Begin() override;
	void End() override;
	void Update([[maybe_unused]] float dt) override
	{

	}
	[[nodiscard]] int GetSystemIndex() const override
	{
		return systemIndex;
	}
	void SetSystemIndex(int index) override
	{
		systemIndex = index;
	}
private:
	int systemIndex = -1;
};
static void RunNetwork();
namespace
{
NetworkSystem networkSystem_;
NetworkClientUi networkClientUi_;
NetworkClientImpl networkClient_;
std::vector<std::pair<std::string, std::string>> regions_;
std::vector<std::function<void()>> networkTasks_;
std::vector<InputPacket> lastReceivedInputPackets_;
std::vector<InputPacket> returnedInputPackets_;
std::vector<ConfirmFramePacket> returnedConfirmPackets_;
std::vector<ConfirmFramePacket> lastReceivedConfirmPackets_;
std::mutex networkTasksMutex_;
std::mutex inputMutex_;
std::mutex confirmPacketMutex_;
neko::FuncJob networkJob_{[](){RunNetwork();}};
std::atomic<NetworkClient::State> state_ = NetworkClient::State::UNCONNECTED;
uint8_t localPlayerIndex_ = std::numeric_limits<uint8_t>::max();

std::atomic<bool> isRunning_ = true;
bool isValid_ = false;
bool isMaster_ = true;
}


void NetworkClientImpl::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
{
	LogDebug(fmt::format("Debug Return: {} with msg: {}", debugLevel, string.ASCIIRepresentation().cstr()));
}
void NetworkClientImpl::connectionErrorReturn(int errorCode)
{
	LogError(fmt::format("Connection Error, code: {}", errorCode));
}
void NetworkClientImpl::clientErrorReturn(int errorCode)
{
	LogError(fmt::format("Client Error, code: {}", errorCode));
}
void NetworkClientImpl::warningReturn(int warningCode)
{
	LogWarning(fmt::format("Warning, code: {}", warningCode));
}
void NetworkClientImpl::serverErrorReturn(int errorCode)
{
	LogError(fmt::format("Server Error, code: {}", errorCode));
}
void NetworkClientImpl::joinRoomEventAction(int playerNr,
	const ExitGames::Common::JVector<int>& playernrs,
	const ExitGames::LoadBalancing::Player& player)
{
	(void) player;
	(void) playerNr;
	(void) playernrs;


	LogDebug(fmt::format("Join Room Event: playerNr: {} player name: {}", playerNr, player.getName().ASCIIRepresentation().cstr()));

	if(state_.load(std::memory_order_acquire) == NetworkClient::State::JOINING)
	{
		state_.store(NetworkClient::State::IN_ROOM, std::memory_order_release);
		localPlayerIndex_ = sixit::guidelines::narrow_cast<uint8_t>(neko::GetLoadBalancingClient().getLocalPlayer().getNumber());
		const auto& room = neko::GetLoadBalancingClient().getCurrentlyJoinedRoom();
		isMaster_ = room.getMasterClientID() == localPlayerIndex_;
	}
}
void NetworkClientImpl::leaveRoomEventAction(int playerNr, bool isInactive)
{
	(void) isInactive;
	(void) playerNr;
	auto& room = neko::GetLoadBalancingClient().getCurrentlyJoinedRoom();
	isMaster_ = room.getMasterClientID() == localPlayerIndex_;
	if(state_.load(std::memory_order_acquire) == NetworkClient::State::IN_GAME)
	{
		state_.store(NetworkClient::State::IN_ROOM);
		room.setIsOpen(true);
	}
}
void NetworkClientImpl::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	(void) playerNr;
	(void) eventContent;
	switch(static_cast<PacketType>(eventCode))
	{

	case PacketType::START_GAME:
	{
		if(state_.load(std::memory_order_acquire) == NetworkClient::State::IN_ROOM)
		{
			state_.store(NetworkClient::State::IN_GAME, std::memory_order_release);
			LogDebug("In Game");
		}
		break;
	}
	case PacketType::INPUT:
	{
		if(state_.load(std::memory_order_acquire) == NetworkClient::State::IN_GAME)
		{
			auto lastReceiveInput_ = ExitGames::Common::ValueObject<InputSerializer>(eventContent).getDataCopy();
			std::scoped_lock<std::mutex> lock(inputMutex_);
			lastReceivedInputPackets_.push_back(lastReceiveInput_.GetPlayerInput());
		}
		break;
	}
	case PacketType::CONFIRM_FRAME:
	{
		if(state_.load(std::memory_order_acquire) == NetworkClient::State::IN_GAME)
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
void NetworkClientImpl::connectReturn(int errorCode,
	const ExitGames::Common::JString& errorString,
	const ExitGames::Common::JString& region,
	const ExitGames::Common::JString& cluster)
{
	(void) errorCode;
	(void) errorString;
	LogDebug(fmt::format("Connect Return: region: {} cluster: {}", region.ASCIIRepresentation().cstr(), cluster.ASCIIRepresentation().cstr()));
	state_.store(NetworkClient::State::CONNECTED_TO_MASTER, std::memory_order_release);
}
void NetworkClientImpl::disconnectReturn(void)
{
	LogDebug("Disconnect Return");
}
void NetworkClientImpl::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	(void)errorCode;
	(void)errorString;
}

void NetworkClientImpl::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& regions,
	const ExitGames::Common::JVector<ExitGames::Common::JString>& regionsServers)
{
	state_.store(NetworkClient::State::CHOOSING_REGIONS);
	std::vector<std::pair<std::string, std::string>> regionsTmp;
	regionsTmp.reserve(regions.getSize());
	for(unsigned i = 0; i < regions.getSize(); i++)
	{
		regionsTmp.emplace_back(regions[i].ASCIIRepresentation().cstr(), regionsServers[i].ASCIIRepresentation().cstr());
	}
	std::swap(regions_, regionsTmp);
}


void BeginNetwork(const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions)
{
	networkTasks_.reserve(15);
	returnedInputPackets_.reserve(10);
	lastReceivedInputPackets_.reserve(10);
	AddSystem(&networkSystem_);
	AddGuiInterface(&networkClientUi_);

	PingSerializer::registerType();
	InputSerializer::registerType();
	ConfirmFrameSerializer::registerType();
	neko::NetworkManager::Begin(&networkClient_, clientConstructOptions);
	isValid_ = true;

}

void NetworkSystem::Begin()
{
}
void NetworkSystem::End()
{
	ConfirmFrameSerializer::unregisterType();
	InputSerializer::unregisterType();
	PingSerializer::unregisterType();
	RemoveSystem(this);
	RemoveGuiInterface(&networkClientUi_);
	isRunning_.store(false, std::memory_order_release);
}

void NetworkClientUi::OnGui()
{
	const auto state = state_.load(std::memory_order_acquire);
	if(state == NetworkClient::State::IN_GAME)
	{
		return;
	}
	ImGui::Begin("Network Client");
	switch(state_.load(std::memory_order_acquire))
	{
	case NetworkClient::State::UNCONNECTED:
	{
		if(ImGui::Button("Connect"))
		{
			ScheduleNetJob(&networkJob_);
			state_.store(NetworkClient::State::CONNECTING, std::memory_order_release);
		}
		break;
	}
	case NetworkClient::State::CONNECTING:
	{
		ImGui::Text("Connecting...");
		break;
	}
	case NetworkClient::State::CONNECTED_TO_MASTER:
	{
		static std::array<char, 40> roomName{};
		static std::array<char, 40> playerName{};
		ImGui::InputText("Player Name", playerName.data(), playerName.size());
		ImGui::InputText("Room Name", roomName.data(), roomName.size());
		if(ImGui::Button("Join Or Create"))
		{
			state_.store(NetworkClient::State::JOINING, std::memory_order_release);
			ExitGames::LoadBalancing::RoomOptions roomOptions{};
			roomOptions.setMaxPlayers(4);
			auto& client = neko::GetLoadBalancingClient();
			auto& player = client.getLocalPlayer();
			player.setName(playerName.data());
			client.opJoinRandomOrCreateRoom(ExitGames::Common::JString(roomName.data()), roomOptions);
		}
		break;
	}
	case NetworkClient::State::JOINING:
	{
		ImGui::Text("Joining...");
		break;
	}
	case NetworkClient::State::CHOOSING_REGIONS:
	{
		ImGui::Text("Choose region:");
		for(const auto& region : regions_)
		{
			if(ImGui::Button(region.first.c_str()))
			{
				std::scoped_lock<std::mutex> lock(networkTasksMutex_);
				networkTasks_.emplace_back([region, this]{
					auto& client = neko::GetLoadBalancingClient();
					client.selectRegion(region.first.c_str());
				});
				break;

			}
		}
		break;
	}
	case NetworkClient::State::IN_ROOM:
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
					state_.store(NetworkClient::State::IN_GAME, std::memory_order_release);

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
void NetworkClientUi::SetGuiIndex(int index)
{
	guiIndex_ = index;
}
int NetworkClientUi::GetGuiIndex() const
{
	return guiIndex_;
}
static void RunNetwork()
{
	static constexpr Uint32 tickTime = 10;//10ms for network loop
	auto previous = SDL_GetTicks();
	while(isRunning_.load(std::memory_order_acquire))
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
		neko::NetworkManager::Tick();
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
	neko::NetworkManager::End();
	isValid_ = false;
#ifdef TRACY_ENABLE
	TracyCZoneEnd(netEnd);
#endif
}
uint8_t NetworkClient::GetPlayerIndex()
{
	return localPlayerIndex_;
}
void SendInputPacket(const InputPacket& inputPacket)
{
	InputSerializer serializer(inputPacket);
	std::scoped_lock<std::mutex> lock(networkTasksMutex_);
	networkTasks_.emplace_back([serializer]
	{
		auto& client = neko::GetLoadBalancingClient();
		ExitGames::LoadBalancing::RaiseEventOptions options{};
		client.opRaiseEvent(false, serializer, static_cast<nByte>(PacketType::INPUT), options);
	});
}
void SendConfirmFramePacket(const ConfirmFramePacket& confirmPacket)
{
	ConfirmFrameSerializer serializer(confirmPacket);
	std::scoped_lock<std::mutex> lock(networkTasksMutex_);
	networkTasks_.emplace_back([serializer]
	{
		auto& client = neko::GetLoadBalancingClient();
		ExitGames::LoadBalancing::RaiseEventOptions options{};
		client.opRaiseEvent(true, serializer, static_cast<nByte>(PacketType::CONFIRM_FRAME), options);
	});
}

neko::Span<InputPacket> GetInputPackets()
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
	const auto& room = neko::GetLoadBalancingClient().getCurrentlyJoinedRoom();
	const auto& players = room.getPlayers();
	for(unsigned i = 0; i < players.getSize(); i++)
	{
		const auto* playerTmp = players[i];
		connectedPlayers[playerTmp->getNumber()-1] = true;
	}
	return connectedPlayers;
}
neko::Span<ConfirmFramePacket> GetConfirmPackets()
{
	{
		std::scoped_lock<std::mutex> lock(confirmPacketMutex_);
		std::swap(lastReceivedConfirmPackets_, returnedConfirmPackets_);
		lastReceivedConfirmPackets_.clear();
	}
	return {returnedConfirmPackets_.data(), returnedConfirmPackets_.size()};
}

bool NetworkClient::IsMaster() {return isMaster_;}
NetworkClient::State NetworkClient::GetState()
{
	return state_.load(std::memory_order_acquire);
}
bool NetworkClient::IsValid()
{
	return isValid_;
}
}