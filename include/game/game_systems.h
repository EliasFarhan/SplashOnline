//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_GAME_SYSTEMS_H
#define SPLASHONLINE_GAME_SYSTEMS_H

#include "game/player_character.h"
#include "game/bullet.h"
#include "game/level.h"

#include <physics/physics.h>

namespace splash
{
class GameSystems : public neko::ContactListener
{
public:
	GameSystems();

	void Begin();
	void Tick();
	void End();

	[[nodiscard]] const PlayerManager& GetPlayerManager() const{return playerManager_;}
	[[nodiscard]] const BulletManager& GetBulletManager() const {return bulletManager_;}

	neko::PhysicsWorld& GetPhysicsWorld(){return physicsManager_;}
	[[nodiscard]] const neko::PhysicsWorld& GetPhysicsWorld() const {return physicsManager_;}
	void OnTriggerEnter(const neko::ColliderPair& p) override;
	void OnTriggerExit(const neko::ColliderPair& p) override;
	void OnCollisionEnter(const neko::ColliderPair& p) override;
	void OnCollisionExit(const neko::ColliderPair& p) override;

private:
	PlayerManager playerManager_;
	BulletManager bulletManager_;
	Level level_;
	neko::PhysicsWorld physicsManager_;
	neko::QuadTree quadTree_;
};
}
#endif //SPLASHONLINE_GAME_SYSTEMS_H
