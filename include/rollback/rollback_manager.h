//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_ROLLBACK_MANAGER_H
#define SPLASHONLINE_ROLLBACK_MANAGER_H

#include "game/game_systems.h"

namespace splash
{

struct GameData;

using RollbackChecksum = Checksum<static_cast<int>(BulletChecksumIndex::LENGTH)+static_cast<int>(PlayerChecksumIndex::LENGTH)>;
struct InputPacket;
class RollbackManager
{
public:
	explicit RollbackManager(const GameData& gameData);
	void Begin();
	void SetInput(uint8_t playerNumber, PlayerInput input, uint16_t currentFrame);
	[[nodiscard]] PlayerInput GetInput(int playerNumber, int currentFrame) const;
	[[nodiscard]] std::pair<std::array<PlayerInput, MaxPlayerInputNmb>, uint8_t> GetInputs(uint8_t playerNumber, uint16_t currentFrame) const;
	[[nodiscard]] std::array<PlayerInput, MaxPlayerNmb> GetInputs(int currentFrame) const;
	[[nodiscard]] RollbackChecksum ConfirmLastFrame();
	[[nodiscard]] int GetLastConfirmFrame() const {return lastConfirmFrame_;}
	[[nodiscard]] int GetLastReceivedFrame() const;
	[[nodiscard]] int GetLastReceivedFrame(int playerNumber) const;
	void SetInputs(const InputPacket& packet);
	[[nodiscard]] bool IsValid(int playerNumber) const;
	[[nodiscard]] bool IsDirty() const { return isDirty_;}
	[[nodiscard]] const GameSystems& GetGameSystems() const;
	void SetDirty(bool dirty);
private:
	struct PlayerInputData
	{
		uint16_t lastReceivedFrame = std::numeric_limits<uint16_t>::max();
		bool isValid = false;
	};
	GameSystems confirmFrameGameSystems_{};
	uint16_t lastConfirmFrame_ = std::numeric_limits<uint16_t>::max();
	std::array<PlayerInputData, MaxPlayerNmb> inputDatas_{};
	std::vector<std::array<PlayerInput, MaxPlayerNmb>> inputs_{};
	bool isDirty_ = false;
};

bool IsValid(int playerNumber);
}
#endif //SPLASHONLINE_ROLLBACK_MANAGER_H

