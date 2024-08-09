//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_GAME_SYSTEMS_H
#define SPLASHONLINE_GAME_SYSTEMS_H

#include "game/player_character.h"
#include "game/bullet.h"
#include "physics/physics.h"


namespace splash
{
class GameSystems
{
public:
	GameSystems();

	PlayerManager& GetPlayerManager(){return playerManager_;}
	BulletManager& GetBulletManager(){return bulletManager_;}
private:
	PlayerManager playerManager_;
	BulletManager bulletManager_;
	neko::PhysicsWorld physicsManager_;
	neko::QuadTree quadTree_;
};
}
#endif //SPLASHONLINE_GAME_SYSTEMS_H
