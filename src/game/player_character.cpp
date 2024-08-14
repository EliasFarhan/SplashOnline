//
// Created by unite on 06.07.2024.
//

#include "game/player_character.h"
#include "game/game_systems.h"
#include "game/const.h"
#include "utils/log.h"

#include <fmt/format.h>

namespace splash
{

static constexpr std::array<neko::Vec2f, MaxPlayerNmb> spawnPositions
	{{
		 {neko::Fixed16{-4.77f}, neko::Fixed16{-1.79f}},
		 {neko::Fixed16{4.13f}, neko::Fixed16{-1.79f}},
		 {neko::Fixed16{-1.65f}, neko::Fixed16{0.96f}},
		 {neko::Fixed16{1.38f}, neko::Fixed16{0.96f}},
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
		collider.userData = &playerPhysic.userData;
		auto& box = physicsWorld.aabb(collider.shapeIndex);
		box.halfSize = PlayerPhysic::box.size/neko::Scalar{2};

		playerPhysic.footColliderIndex = physicsWorld.AddAabbCollider(playerPhysic.bodyIndex);
		auto& footCollider = physicsWorld.collider(playerPhysic.footColliderIndex);
		footCollider.isTrigger = true;
		footCollider.offset = PlayerPhysic::footBox.offset;
		footCollider.userData = &playerPhysic.userData;
		auto& footBox = physicsWorld.aabb(footCollider.shapeIndex);
		footBox.halfSize = PlayerPhysic::footBox.size/neko::Scalar{2};

		playerPhysic.userData.type = ColliderType::PLAYER;
		playerPhysic.userData.playerNumber = playerIndex;


	}
}
void PlayerManager::Tick()
{
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		auto& playerCharacter = playerCharacters_[playerNumber];
		auto& playerInput = playerInputs_[playerNumber];
		auto& playerPhysic = playerPhysics_[playerNumber];
		auto& physicsWorld = gameSystems_->GetPhysicsWorld();
		auto& body = physicsWorld.body(playerPhysic.bodyIndex);


		if(playerCharacter.footCount > 0)
		{
			//on ground
			const auto moveX = neko::Abs(playerInput.moveDirX) > PlayerCharacter::deadZone ? neko::Scalar{playerInput.moveDirX} : neko::Scalar{};
			const auto wantedSpeed = moveX * PlayerCharacter::WalkSpeed;
			const auto velX = body.velocity.x;
			const auto deltaSpeed = wantedSpeed-velX;
			const auto newCap = PlayerCharacter::CapMoveForce;
			//TODO add wet cap
			if(playerPhysic.priority <= PlayerCharacter::MovePriority)
			{
				if(playerPhysic.priority < PlayerCharacter::MovePriority)
				{
					playerPhysic.priority = PlayerCharacter::MovePriority;
					//Counter gravity
					playerPhysic.totalForce = -physicsWorld.gravity()/body.inverseMass;
				}

				auto f = deltaSpeed / fixedDeltaTime / body.inverseMass;

				if(newCap > neko::Fixed16{} && neko::Abs(f) > newCap)
				{
					f = newCap * neko::Sign(deltaSpeed) ;
				}
				playerPhysic.totalForce.x = f;

			}

		}
		else
		{
			//in air
			const auto reactor = neko::Scalar {playerInput.moveDirY};
			if(reactor > PlayerCharacter::ReactorInAirThreshold)
			{
				const auto velY = body.velocity.y;
				const auto decreaseFactor = velY > neko::Scalar{} ? neko::Scalar{ 0.75f } : neko::Scalar{ 1.0f };

				auto force = PlayerCharacter::ReactorForce * reactor * decreaseFactor;
				//TODO jetburst and jumping
				if(playerPhysic.priority <= PlayerCharacter::JetPackPriority)
				{
					playerPhysic.totalForce.y = force;
				}
			}
		}

		// In the end, apply force to physics
		body.force = playerPhysic.totalForce;
		playerPhysic.totalForce = {};
		playerPhysic.priority = {};
	}
}
void PlayerManager::End()
{

}
void PlayerManager::SetPlayerInput(neko::Span<PlayerInput> playerInputs)
{
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		playerInputs_[i] = playerInputs[i];
	}
}
void PlayerManager::OnTriggerExit(neko::ColliderIndex playerIndex,
	int playerNumber,
	const neko::Collider& otherCollider)
{
	const auto* otherUserData = static_cast<ColliderUserData*>(otherCollider.userData);
	if(playerIndex == playerPhysics_[playerNumber].footColliderIndex && otherUserData->type == ColliderType::PLATFORM)
	{
		playerCharacters_[playerNumber].footCount--;
	}
}
void PlayerManager::OnTriggerEnter(neko::ColliderIndex playerIndex, int playerNumber, const neko::Collider& otherCollider)
{
	const auto* otherUserData = static_cast<ColliderUserData*>(otherCollider.userData);
	if(playerIndex == playerPhysics_[playerNumber].footColliderIndex && otherUserData->type == ColliderType::PLATFORM)
	{
		playerCharacters_[playerNumber].footCount++;
	}
}
}