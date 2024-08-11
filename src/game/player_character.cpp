//
// Created by unite on 06.07.2024.
//

#include "game/player_character.h"
#include "game/game_systems.h"
#include "game/const.h"

namespace splash
{

static constexpr std::array<neko::Vec2f, MaxPlayerNmb> spawnPositions
	{{
		 {neko::Fixed16{-4.77f}, neko::Fixed16{-1.6f}},
		 {neko::Fixed16{4.13f}, neko::Fixed16{-1.79f}},
		 {neko::Fixed16{-1.65f}, neko::Fixed16{0.86f}},
		 {neko::Fixed16{1.38f}, neko::Fixed16{0.53f}},
	}};

PlayerManager::PlayerManager(GameSystems* gameSystems): gameSystems_(gameSystems)
{

}
void PlayerManager::Begin()
{
	auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	for(int playerIndex = 0; playerIndex < MaxPlayerNmb; playerIndex++)
	{
		auto& playerPhysic = playerPhysics_[playerIndex];
		playerPhysic.bodyIndex = physicsWorld.AddBody();
		playerPhysic.colliderIndex = physicsWorld.AddAabbCollider(playerPhysic.bodyIndex);

		auto& body = physicsWorld.body(playerPhysic.bodyIndex);
		body.position = spawnPositions[playerIndex];
		body.type = neko::BodyType::DYNAMIC;
		body.inverseMass = neko::Scalar{1};

		auto& collider = physicsWorld.collider(playerPhysic.colliderIndex);
		collider.isTrigger = false;
		collider.offset = PlayerPhysic::box.offset;
		collider.restitution = {};
		auto& box = physicsWorld.aabb(collider.shapeIndex);
		box.halfSize = PlayerPhysic::box.size/neko::Scalar{2};

	}
}
void PlayerManager::Tick()
{

}
void PlayerManager::End()
{

}
}