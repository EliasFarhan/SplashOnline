//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_ROLLBACK_MANAGER_H
#define SPLASHONLINE_ROLLBACK_MANAGER_H

#include "game/game_systems.h"
#include "network/packet.h"

namespace splash
{

struct GameData;
class RollbackManager
{
public:
	RollbackManager(const GameData& gameData);
	void Begin();
	void SetInput(int playerNumber, PlayerInput input, int currentFrame);
	[[nodiscard]] PlayerInput GetInput(int playerNumber, int currentFrame) const;
	[[nodiscard]] std::pair<std::array<PlayerInput, MaxPlayerInputNmb>, int> GetInputs(int playerNumber, int currentFrame) const;
	[[nodiscard]] std::array<PlayerInput, MaxPlayerNmb> GetInputs(int currentFrame) const;
	[[nodiscard]] std::uint32_t ConfirmLastFrame();
	[[nodiscard]] int GetLastConfirmFrame() const {return lastConfirmFrame_;}
	[[nodiscard]] int GetLastReceivedFrame() const;
	[[nodiscard]] int GetLastReceivedFrame(int playerNumber) const;
	void SetInputs(const InputPacket& packet);
	[[nodiscard]] bool IsValid(int playerNumber) const;
	[[nodiscard]] bool IsDirty() const { return isDirty_;}
	const GameSystems& GetGameSystems() const;
	void SetDirty(bool dirty);
private:
	struct PlayerInputData
	{
		int lastReceivedFrame = -1;
		bool isValid = false;
	};
	GameSystems confirmFrameGameSystems_{};
	int lastConfirmFrame_ = -1;
	std::array<PlayerInputData, MaxPlayerNmb> inputDatas_{};
	std::vector<std::array<PlayerInput, MaxPlayerNmb>> inputs_{};
	bool isDirty_ = false;
};

bool IsValid(int playerNumber);
}
#endif //SPLASHONLINE_ROLLBACK_MANAGER_H

