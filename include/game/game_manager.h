//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_GAME_MANAGER_H_
#define SPLASHONLINE_GAME_GAME_MANAGER_H_

#include "rollback/rollback_manager.h"
#include "game/graphics/game_view.h"
#include "engine/system.h"

namespace splash
{

struct GameData
{
	std::array<bool, MaxPlayerNmb> connectedPlayers{};
	float introDelay = 5.0f;
	neko::Scalar period {30.0f}; //two minutes default game
};

class GameManager final : public SystemInterface
{
public:
	explicit GameManager(const GameData&  = {});
	void Begin() override;
	void Update(float dt) override;
	void End() override;
	void Tick();
	[[nodiscard]] int GetSystemIndex() const override;
	void SetSystemIndex(int index) override;
private:
	void RollbackUpdate();
	GameSystems gameSystems_;
	GameView gameRenderer_;
	RollbackManager rollbackManager_;
	Timer<float> introDelayTimer_;
	Timer<> gameTimer_;
	float currentTime_ = 0.0f;
	int currentFrame_ = -1;
	int systemIndex_ = -1;
	std::array<bool, MaxPlayerNmb> connectedPlayers_{};
	std::array<PlayerInput, MaxPlayerNmb> playerInputs_;

	bool isGameOver_ = false;
};
}
#endif //SPLASHONLINE_GAME_GAME_MANAGER_H_
