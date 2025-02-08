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
	[[nodiscard]] neko::Vec2i GetPlayerScreenPos() const;
	[[nodiscard]] bool HasDesync() const {return hasDesync_;}
	[[nodiscard]] int GetCurrentFrame() const {return currentFrame_;}
private:
	void RollbackUpdate();
	GameSystems gameSystems_;
	GameView gameRenderer_;
	RollbackManager rollbackManager_;
	RunTimeTimer<float> introDelayTimer_;
	RunTimeTimer<neko::Scalar> gameTimer_;
	float currentTime_ = 0.0f;
	uint16_t currentFrame_ = std::numeric_limits<uint16_t>::max();
	int systemIndex_ = -1;
	std::array<PlayerInput, MaxPlayerNmb> playerInputs_;

	bool isGameOver_ = false;
	bool hasDesync_ = false;

	void ExitGame();
};

neko::Vec2i GetPlayerScreenPos();
int GetCurrentFrame();


}
#endif //SPLASHONLINE_GAME_GAME_MANAGER_H_
