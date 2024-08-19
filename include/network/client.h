//
// Created by unite on 04.08.2024.
//

#ifndef SPLASHONLINE_NETWORK_CLIENT_H_
#define SPLASHONLINE_NETWORK_CLIENT_H_

#include "engine/system.h"
#include "graphics/gui_renderer.h"

#include <neko/network_manager.h>
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
		CONNECTED_TO_MASTER,
		JOINING,
		IN_ROOM,
		IN_GAME,
	};

	NetworkClient();
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

	void Begin() override;
	void End() override;
	void Update(float dt) override;

	[[nodiscard]] int GetSystemIndex() const override;
	void SetSystemIndex(int index) override;
	void OnGui() override;
	void SetGuiIndex(int index) override;
	[[nodiscard]] int GetGuiIndex() const override;

	[[nodiscard]] State GetState() const { return state_.load(std::memory_order_consume); }
	int GetPlayerIndex();
private:
	void RunNetwork();

	neko::NetworkManager networkManager_;
	neko::FuncJob networkJob_;
	std::atomic<State> state_ = State::UNCONNECTED;
	int systemIndex_ = -1;
	int guiIndex_ = -1;
	int localPlayerIndex_ = -1;
	std::atomic<bool> isRunning_ = true;
};

NetworkClient* GetNetworkClient();
}

#endif //SPLASHONLINE_NETWORK_CLIENT_H_
