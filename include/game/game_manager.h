//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_GAME_MANAGER_H_
#define SPLASHONLINE_GAME_GAME_MANAGER_H_

#include "rollback/rollback_manager.h"
#include "game/graphics/game_renderer.h"
#include "engine/system.h"

namespace splash
{
class GameManager : public SystemInterface
{
public:
	GameManager();
	void Begin() override;
	void Update(float dt) override;
	void End() override;
	void Tick();
	[[nodiscard]] int GetSystemIndex() const override;
	void SetSystemIndex(int index) override;
private:
	GameSystems gameSystems_;
	GameRenderer gameRenderer_;
	RollbackManager rollbackManager_;
	float currentTime_ = 0.0f;
	int systemIndex_ = 0;
	std::array<PlayerInput, MaxPlayerNmb> playerInputs_;
};
}
#endif //SPLASHONLINE_GAME_GAME_MANAGER_H_
