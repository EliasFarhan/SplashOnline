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
		body.isActive = false;

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

		const auto reactor = neko::Scalar {playerInput.moveDirY};
		const auto moveX = neko::Abs(playerInput.moveDirX) > PlayerCharacter::deadZone ? neko::Scalar{playerInput.moveDirX} : neko::Scalar{};


		if(!playerCharacter.respawnStaticTime.Over())
		{
			playerCharacter.respawnStaticTime.Update(fixedDeltaTime);
			if(playerCharacter.respawnStaticTime.Over() ||
				neko::Abs(playerInput.moveDirX) > PlayerCharacter::deadZone ||
				reactor > PlayerCharacter::ReactorThreshold)
			{
				body.position = spawnPositions[playerNumber];
				body.isActive = true;
				playerCharacter.respawnStaticTime.Stop();
			}
			else
			{
				continue;
			}
		}
		if(!playerCharacter.respawnMoveTimer.Over())
		{
			playerCharacter.respawnMoveTimer.Update(fixedDeltaTime);
			if(playerCharacter.respawnMoveTimer.Over())
			{
				playerCharacter.respawnStaticTime.Reset();
			}
			continue;
		}
		if(!playerCharacter.respawnPauseTimer.Over())
		{
			playerCharacter.respawnPauseTimer.Update(fixedDeltaTime);
			if(playerCharacter.respawnPauseTimer.Over())
			{
				playerCharacter.respawnMoveTimer.Reset();
			}
			continue;
		}

		if(playerCharacter.jetBurstTimer.Over() &&
			(reactor < PlayerCharacter::ReactorThreshold ||
			body.velocity.y > neko::Scalar{0.0f}))
		{
			playerCharacter.burstTimer.Update(fixedDeltaTime);
		}
		if(!playerCharacter.jetBurstTimer.Over())
		{
			playerCharacter.jetBurstTimer.Update(fixedDeltaTime);
			neko::Scalar force{};
			if(playerCharacter.jetBurstTimer.Over())
			{
				const auto jumpSpeed = PlayerCharacter::JumpForce*body.inverseMass*fixedDeltaTime;
				force = jumpSpeed-body.velocity.y / fixedDeltaTime / body.inverseMass;
				playerCharacter.jumpTimer.Reset();
			}
			else
			{
				force = PlayerCharacter::ReactorForce;
			}
			if(playerPhysic.priority < PlayerCharacter::JetPackPriority )
			{
				playerPhysic.priority = PlayerCharacter::JetPackPriority;
				playerPhysic.totalForce.y = force;
			}
		}
		if(playerCharacter.IsGrounded())
		{
			//on ground
			if(reactor > PlayerCharacter::JetBurstThreshold)
			{
				if(playerCharacter.burstTimer.Over() && playerCharacter.jetBurstTimer.Over())
				{
					playerCharacter.jetBurstTimer.Reset();
					playerCharacter.burstTimer.Reset();
				}
			}

			const auto wantedSpeed = moveX * PlayerCharacter::WalkSpeed;
			const auto velX = body.velocity.x;
			const auto deltaSpeed = wantedSpeed-velX;
			const auto newCap = PlayerCharacter::CapMoveForce;
			//TODO add wet cap
			if(playerPhysic.priority <= PlayerCharacter::MovePriority)
			{
				if(playerPhysic.priority < PlayerCharacter::MovePriority )
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
			if(playerPhysic.priority <= PlayerCharacter::MovePriority)
			{
				const auto horizontalForce = PlayerCharacter::InAirForce * moveX;
				playerPhysic.totalForce += neko::Vec2f{horizontalForce, {}};
				playerPhysic.priority = PlayerCharacter::MovePriority;
			}
			//in air
			if(reactor > PlayerCharacter::ReactorThreshold &&
			   playerCharacter.jetBurstTimer.Over())
			{
				const auto velY = body.velocity.y;
				const auto decreaseFactor = velY > neko::Scalar{} ? neko::Scalar{ 0.75f } : neko::Scalar{ 1.0f };

				auto force = PlayerCharacter::ReactorForce * reactor * decreaseFactor;
				//TODO jetburst and jumping


				if(!playerCharacter.jumpTimer.Over())
				{
					playerCharacter.jumpTimer.Update(fixedDeltaTime);

					const auto t = playerCharacter.jumpTimer.CurrentRatio();
					force *= (t*t*t*t);
				}
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
	const auto* otherUserData = static_cast<const ColliderUserData*>(otherCollider.userData);
	if(playerIndex == playerPhysics_[playerNumber].footColliderIndex &&
		otherUserData->type == ColliderType::PLATFORM)
	{
		playerCharacters_[playerNumber].footCount--;
	}
	if(otherUserData->type == ColliderType::GAME_LIMIT)
	{
		Respawn(playerNumber);
	}
	if(otherUserData->type == ColliderType::BULLET)
	{
		//TODO manage wata hit
	}
}
void PlayerManager::OnTriggerEnter(neko::ColliderIndex playerIndex, int playerNumber, const neko::Collider& otherCollider)
{
	const auto* otherUserData = static_cast<const ColliderUserData*>(otherCollider.userData);
	if(playerIndex == playerPhysics_[playerNumber].footColliderIndex &&
		otherUserData->type == ColliderType::PLATFORM)
	{
		playerCharacters_[playerNumber].footCount++;
	}
}

void PlayerManager::Respawn(int playerNumber)
{
	auto& playerCharacter = playerCharacters_[playerNumber];
	if(!playerCharacter.respawnPauseTimer.Over() || !playerCharacter.respawnMoveTimer.Over())
	{
		return;
	}
	auto& playerPhysic = playerPhysics_[playerNumber];
	auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	auto& body = physicsWorld.body(playerPhysic.bodyIndex);

	body.isActive = false;
	playerCharacter.respawnPauseTimer.Reset();
}
}