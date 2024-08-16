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
		auto target = neko::Vec2f{neko::Scalar {playerInput.targetDirX}, neko::Scalar {playerInput.targetDirY}};

		playerCharacter.jumpTimer.Update(fixedDeltaTime);

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

		if(playerCharacter.preJetBurstTimer.Over() &&
			(reactor < PlayerCharacter::ReactorThreshold ||
			body.velocity.y > neko::Scalar{0.0f}))
		{
			playerCharacter.jetBurstCoolDownTimer.Update(fixedDeltaTime);
		}

		//Walk Update
		if(playerCharacter.IsGrounded())
		{
			//on ground
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
		// In Air Move and not dashing!!!
		if(!playerCharacter.IsGrounded())
		{
			if(playerPhysic.priority <= PlayerCharacter::MovePriority)
			{
				const auto horizontalForce = PlayerCharacter::InAirForce * moveX;
				playerPhysic.totalForce += neko::Vec2f{horizontalForce, {}};
				playerPhysic.priority = PlayerCharacter::MovePriority;
			}
		}
		//Jetpack update
		bool jetbursting = playerCharacter.IsJetBursting();
		if(!jetbursting && (reactor < PlayerCharacter::ReactorThreshold || body.velocity.y < neko::Scalar {0.0f}))
		{
			playerCharacter.jetBurstCoolDownTimer.Update(fixedDeltaTime);
		}
		if(jetbursting)
		{
			playerCharacter.preJetBurstTimer.Update(fixedDeltaTime);
		}
		if((playerCharacter.IsGrounded() && reactor > PlayerCharacter::GroundReactorThreshold) ||
			(!playerCharacter.IsGrounded() && reactor > PlayerCharacter::ReactorThreshold))
		{
			const auto velY = body.velocity.y;
			const auto decreaseFactor = velY > neko::Scalar{} ? neko::Scalar{ 0.75f } : neko::Scalar{ 1.0f };
			auto force = PlayerCharacter::ReactorForce * reactor * decreaseFactor;
			if(playerCharacter.jetBurstCoolDownTimer.Over() && !jetbursting && reactor > PlayerCharacter::JetBurstThreshold) //TODO add collided
			{
				jetbursting = true;
				playerCharacter.preJetBurstTimer.Reset();
				playerCharacter.jetBurstCoolDownTimer.Reset();
			}
			if(jetbursting)
			{
				force = PlayerCharacter::ReactorForce;
			}

			if((playerCharacter.IsGrounded() || jetbursting && playerCharacter.preJetBurstTimer.Over()) &&
				playerCharacter.jumpTimer.Over() &&
				velY < PlayerCharacter::JumpForce*body.inverseMass*fixedDeltaTime && body.velocity.Length() < neko::Scalar {12.0f})
			{
				//Doing a jet burst
				const auto jumpSpeed = PlayerCharacter::JumpForce*body.inverseMass*fixedDeltaTime;
				force = jumpSpeed-body.velocity.y / fixedDeltaTime / body.inverseMass;
				playerCharacter.jumpTimer.Reset();
			}

			if(!playerCharacter.jumpTimer.Over())
			{
				const auto t = neko::Scalar {1}-playerCharacter.jumpTimer.CurrentRatio();
				force *= (t*t*t*t);
			}

			//TODO reset force at 0 if collided and not on ground
			if(playerPhysic.priority <= PlayerCharacter::JetPackPriority)
			{
				playerPhysic.totalForce += neko::Vec2f{{}, force};
				playerPhysic.priority = PlayerCharacter::JetPackPriority;
			}
			if(velY > neko::Scalar {0.0f})
			{
				playerCharacter.jetBurstCoolDownTimer.Reset();
			}
		}
		else if(playerCharacter.IsGrounded() || reactor < PlayerCharacter::ReactorThreshold)
		{
			jetbursting = false;
			playerCharacter.preJetBurstTimer.Stop();
			playerCharacter.jetBurstCoolDownTimer.Stop();
		}
		if((reactor < PlayerCharacter::ReactorThreshold || body.velocity.y < neko::Fixed{0.0f}) && playerCharacter.jumpTimer.RemainingTime() < PlayerCharacter::JumpCancelTime)
		{
			playerCharacter.jumpTimer.Stop();
		}

		//TODO stomp update

		//TODO Cap velocity update

		//Water gun update
		if(playerCharacter.reserveWaterTimer.Over())
		{
			playerCharacter.reloadTimer.Reset();
		}
		if(playerCharacter.IsReloading())
		{
			target = {};
			playerCharacter.reloadTimer.Update(fixedDeltaTime);
		}

		if(target.Length() < neko::Scalar {PlayerCharacter::deadZone})
		{
			playerCharacter.reserveWaterTimer.Update(
				-fixedDeltaTime * playerCharacter.reserveWaterTimer.GetPeriod() /
				playerCharacter.reloadTimer.GetPeriod());
		}
		else
		{
			if(playerCharacter.firstShots > 0)
			{
				playerCharacter.reserveWaterTimer.Update(fixedDeltaTime*PlayerCharacter::FirstShotFactor);
			}
			else
			{
				playerCharacter.reserveWaterTimer.Update(fixedDeltaTime);
			}
		}

		if(playerCharacter.reserveWaterTimer.RemainingTime() > playerCharacter.reserveWaterTimer.GetPeriod())
		{
			playerCharacter.reserveWaterTimer.Reset();
			playerCharacter.reloadTimer.Stop();
			playerCharacter.firstShots = PlayerCharacter::FirstShotsCount;
		}
		if((!playerCharacter.waterTimer.Over() && playerCharacter.firstShots == 0) ||
			(playerCharacter.firstShots > 0 && playerCharacter.waterTimer.CurrentRatio() < PlayerCharacter::FirstShotRatio))
		{
			playerCharacter.waterTimer.Update(fixedDeltaTime);
		}
		else
		{
			if(target.SquareLength() > neko::Scalar {PlayerCharacter::deadZone})
			{
				//TODO not shooting if stomp prep or dashing
				//Shoot wata bullet
				auto& bulletManager = gameSystems_->GetBulletManager();
				const auto speedFactor = playerCharacter.firstShots>0?neko::Scalar {1.0f}:playerCharacter.reserveWaterTimer.RemainingTime();
				bulletManager.SpawnWata(body.position+PlayerCharacter::WataOffsetPos+target, target.Normalized(), playerNumber, playerCharacter.firstShots>0,speedFactor);
				playerCharacter.waterTimer.Reset();
				if(playerCharacter.firstShots > 0)
				{
					playerCharacter.firstShots--;
				}
			}
		}

		// In the end, apply force to physics
		body.force = playerPhysic.totalForce;
		playerPhysic.totalForce = {};
		playerPhysic.priority = 0;
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
	body.velocity = {};
	body.force = {};
	body.isActive = false;
	playerCharacter.respawnPauseTimer.Reset();
}
}