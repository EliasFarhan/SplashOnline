//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_GAME_SYSTEMS_H
#define SPLASHONLINE_GAME_SYSTEMS_H

#include "game/player_character.h"
#include "game/bullet.h"
#include "game/level.h"
#include "engine/input_manager.h"
#include "rollback/rollback_system.h"

#include <physics/physics.h>

namespace splash
{


class GameSystems : public neko::ContactListener, public RollbackInterface<GameSystems>
{
public:
	GameSystems();

	void Begin();
	void Tick();
	void End();

	void SetPlayerInput(neko::Span<PlayerInput> playerInputs);

	[[nodiscard]] const PlayerManager& GetPlayerManager() const{return playerManager_;}
	[[nodiscard]] const BulletManager& GetBulletManager() const {return bulletManager_;}
	[[nodiscard]] BulletManager& GetBulletManager() {return bulletManager_;}

	neko::PhysicsWorld& GetPhysicsWorld(){return physicsManager_;}
	const auto& GetQuadTree() const {return quadTree_;}
	[[nodiscard]] const neko::PhysicsWorld& GetPhysicsWorld() const {return physicsManager_;}
	void OnTriggerEnter(const neko::ColliderPair& p) override;
	void OnTriggerExit(const neko::ColliderPair& p) override;
	void OnCollisionEnter(const neko::ColliderPair& p) override;
	void OnCollisionExit(const neko::ColliderPair& p) override;

	uint32_t CalculateChecksum() const override;

	void RollbackFrom(const GameSystems& system) override;

private:
	PlayerManager playerManager_;
	BulletManager bulletManager_;
	Level level_;
	neko::PhysicsWorld physicsManager_;
	neko::QuadTree quadTree_;
};
}
#endif //SPLASHONLINE_GAME_SYSTEMS_H
