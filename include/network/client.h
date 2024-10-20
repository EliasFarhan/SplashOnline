//
// Created by unite on 04.08.2024.
//

#ifndef SPLASHONLINE_NETWORK_CLIENT_H_
#define SPLASHONLINE_NETWORK_CLIENT_H_

#include "engine/system.h"
#include "graphics/gui_renderer.h"
#include "network/packet.h"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#endif
#include <neko/network_manager.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include <container/span.h>
#include <thread/job_system.h>

namespace splash
{

class NetworkClient : public SystemInterface, public OnGuiInterface, public neko::ClientInterface
{
public:

	enum class State
	{
		UNCONNECTED,
		CONNECTING,
		CHOOSING_REGIONS,
		CONNECTED_TO_MASTER,
		JOINING,
		IN_ROOM,
		IN_GAME,
	};

	explicit NetworkClient(const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions={});
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
	void disconnectReturn() override;
	void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override;
	void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString> & regions,
		const ExitGames::Common::JVector<ExitGames::Common::JString> & regionsServers) override;

	void Begin() override;
	void End() override;
	void Update(float dt) override;

	[[nodiscard]] int GetSystemIndex() const override;
	void SetSystemIndex(int index) override;
	void OnGui() override;
	void SetGuiIndex(int index) override;
	[[nodiscard]] int GetGuiIndex() const override;

	[[nodiscard]] State GetState() const { return state_.load(std::memory_order_consume); }
	int GetPlayerIndex() const;
	std::array<bool, MaxPlayerNmb> GetConnectedPlayers();

	void SendInputPacket(const InputPacket& inputPacket);
	void SendConfirmFramePacket(const ConfirmFramePacket& confirmPacket);
	neko::Span<InputPacket> GetInputPackets();
	[[nodiscard]] bool IsMaster() const{return isMaster_;}
	neko::Span<ConfirmFramePacket> GetConfirmPackets();
private:
	void RunNetwork();

	neko::NetworkManager networkManager_;
	std::vector<std::pair<std::string, std::string>> regions_;
	std::vector<std::function<void()>> networkTasks_;
	std::vector<InputPacket> lastReceivedInputPackets_;
	std::vector<InputPacket> returnedInputPackets_;
	std::vector<ConfirmFramePacket> returnedConfirmPackets_;
	std::vector<ConfirmFramePacket> lastReceivedConfirmPackets_;
	std::mutex networkTasksMutex_;
	std::mutex inputMutex_;
	std::mutex confirmPacketMutex_;
	neko::FuncJob networkJob_;
	std::atomic<State> state_ = State::UNCONNECTED;
	int systemIndex_ = -1;
	int guiIndex_ = -1;
	int localPlayerIndex_ = -1;

	std::atomic<bool> isRunning_ = true;

	bool isMaster_ = true;
};

NetworkClient* GetNetworkClient();
}

#endif //SPLASHONLINE_NETWORK_CLIENT_H_
