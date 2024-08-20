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
	}
}
void NetworkClient::leaveRoomEventAction(int playerNr, bool isInactive)
{
	(void) playerNr;
	(void) isInactive;

}
void NetworkClient::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	(void) playerNr;
	(void) eventContent;
	switch((PacketType)eventCode)
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
			lastReceiveInput_ = ExitGames::Common::ValueObject<InputSerializer>(eventContent).getDataCopy();
		}
		break;
	}
	case PacketType::CONFIRM_FRAME:
	{
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
				networkTasks_.push_back(std::make_unique<neko::FuncJob>([region, this]{
					auto& client = networkManager_.GetClient();
					client.selectRegion(region.first.c_str());
				}));
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
		auto& player1 = client.getLocalPlayer();
		auto& room = client.getCurrentlyJoinedRoom();
		const auto playerCount = room.getPlayerCount();
		ImGui::Text("Player Count: %d", room.getPlayerCount());
		ImGui::Text("Player Nbr: %d", localPlayerIndex_);
		if(playerCount >= 2 && room.getMasterClientID() == player1.getNumber())
		{
			if(ImGui::Button("Start Game"))
			{
				std::scoped_lock<std::mutex> lock(networkTasksMutex_);
				networkTasks_.push_back(std::make_unique<neko::FuncJob>([this]{
					state_.store(State::IN_GAME, std::memory_order_release);


					auto& client = neko::GetLoadBalancingClient();
					auto& room = client.getCurrentlyJoinedRoom();
					room.setIsOpen(false);
					ExitGames::LoadBalancing::RaiseEventOptions options{};
					client.opRaiseEvent(true, (nByte)0, (nByte)PacketType::START_GAME, options);
				}));
				//todo create the game manager?
			}
		}

		break;
	}
	case State::IN_GAME:
	{
		ImGui::Text("In Game!");
		const auto& playerInputPacket = lastReceiveInput_.GetPlayerInput();
		const auto& playerInput = playerInputPacket.inputs[0];
		ImGui::Text("Input: Move(%1.2f, %1.2f), Target(%1.2f, %1.2f)",
			(float)playerInput.moveDirX,
			(float)playerInput.moveDirY,
			(float)playerInput.targetDirX,
			(float)playerInput.targetDirY);
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
				networkTask->Execute();
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
int NetworkClient::GetPlayerIndex()
{
	if(state_.load(std::memory_order_consume) == State::IN_GAME)
	{
		return networkManager_.GetClient().getLocalPlayer().getNumber();
	}
	return 0;
}
void NetworkClient::SendInputPacket(const InputPacket& inputPacket)
{
	InputSerializer serializer(inputPacket);
	std::scoped_lock<std::mutex> lock(networkTasksMutex_);
	networkTasks_.push_back(std::make_unique<neko::FuncJob>([this, serializer]
	{
		auto& client = networkManager_.GetClient();
		ExitGames::LoadBalancing::RaiseEventOptions options{};
		client.opRaiseEvent(false, serializer, (nByte)PacketType::INPUT, options);
	}));
}
void NetworkClient::SendConfirmFramePacket([[maybe_unused]] const ConfirmFramePacket& confirmPacket)
{
	ConfirmFrameSerializer serializer(confirmPacket);
	std::scoped_lock<std::mutex> lock(networkTasksMutex_);
	networkTasks_.push_back(std::make_unique<neko::FuncJob>([this, serializer]
	{
		auto& client = networkManager_.GetClient();
		ExitGames::LoadBalancing::RaiseEventOptions options{};
		client.opRaiseEvent(true, serializer, (nByte)PacketType::CONFIRM_FRAME, options);
	}));
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
NetworkClient* GetNetworkClient()
{
	return instance;
}
}