//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_GAME_MANAGER_H_
#define SPLASHONLINE_GAME_GAME_MANAGER_H_

#include "rollback/rollback_manager.h"

namespace splash
{
class GameManager
{
public:
	void Begin();
	void Update(float dt);
	void End();
	void Tick();
private:
	GameSystems gameSystems_;
	RollbackManager rollbackManager_;
	float currentTime_ = 0.0f;
};
}
#endif //SPLASHONLINE_GAME_GAME_MANAGER_H_
