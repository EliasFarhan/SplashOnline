//
// Created by unite on 06.07.2024.
//

#include "game/player_character.h"
#include "game/game_systems.h"
#include "game/const.h"
#include "utils/log.h"
#include "rollback/rollback_manager.h"

#include <fmt/format.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

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
		if(!IsValid(playerIndex))
		{
			continue;
		}
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

		playerPhysic.headColliderIndex = physicsWorld.AddAabbCollider(playerPhysic.bodyIndex);
		auto& headCollider = physicsWorld.collider(playerPhysic.headColliderIndex);
		headCollider.isTrigger = true;
		headCollider.offset = PlayerPhysic::headBox.position+PlayerPhysic::headBox.offset;
		headCollider.userData = &playerPhysic.userData;
		auto& headBox = physicsWorld.aabb(headCollider.shapeIndex);
		headBox.halfSize = PlayerPhysic::headBox.size/neko::Scalar{2};

		playerPhysic.userData.type = ColliderType::PLAYER;
		playerPhysic.userData.playerNumber = playerIndex;


	}
}
void PlayerManager::Tick()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{

		if(!IsValid(playerNumber))
		{
			continue;
		}
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
		if(!playerCharacter.IsGrounded() && neko::Abs(moveX) > neko::Scalar{PlayerCharacter::deadZone})
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
				velY < PlayerCharacter::JumpForce*body.inverseMass*fixedDeltaTime &&
				body.velocity.Length() < PlayerCharacter::StompOrBurstMaxVelocity)
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

		//stomp update
		playerCharacter.dashPrepTimer.Update(fixedDeltaTime);
		if(!playerCharacter.dashDownTimer.Over())
		{
			playerCharacter.dashDownTimer.Update(fixedDeltaTime);
			if(playerCharacter.dashDownTimer.Over())
			{
				//Finish stomp
				playerCharacter.slowDashTimer.Reset();
				playerCharacter.jetBurstCoolDownTimer.Stop();
			}
		}
		if(!playerCharacter.slowDashTimer.Over())
		{
			if(playerCharacter.IsGrounded())
			{
				playerCharacter.slowDashTimer.Stop();
			}
			else
			{
				playerCharacter.slowDashTimer.Update(fixedDeltaTime);
				if(playerPhysic.priority <= PlayerCharacter::SlowDashPriority)
				{
					playerPhysic.totalForce.y += PlayerCharacter::SlowDashForce;
					playerPhysic.priority = PlayerCharacter::SlowDashPriority;
				}
			}
		}
		if(!playerCharacter.IsGrounded())
		{
			if(!playerCharacter.IsDashing() &&
				!playerCharacter.IsDashPrepping() &&
				reactor < PlayerCharacter::FallingThreshold) //TODO add stopdash
			{
				if(playerPhysic.priority <= PlayerCharacter::FallingPriority)
				{
					playerPhysic.totalForce.y += PlayerCharacter::FallingForce * reactor;
				}
			}
			if(playerInput.GetStomp() &&
				!previousPlayerInputs_[playerNumber].GetStomp() &&
				!playerCharacter.IsDashing() &&
				!playerCharacter.IsDashPrepping() &&
				!playerCharacter.IsDashed() &&
				body.velocity.Length() < PlayerCharacter::StompOrBurstMaxVelocity) // todo not collided
			{
				//Start stomp prep
				playerCharacter.dashPrepTimer.Reset();
			}
			else if(playerCharacter.IsDashing())
			{
				//Update stomp with moveX
				neko::Vec2f vel{moveX*PlayerCharacter::DashSpeed, -PlayerCharacter::DashSpeed};
				if(playerPhysic.priority < PlayerCharacter::DashPriority)
				{
					playerPhysic.totalForce = (vel-body.velocity)/fixedDeltaTime/body.inverseMass;
					playerPhysic.priority = PlayerCharacter::DashPriority;
				}

			}
			else if(playerCharacter.IsDashPrepping() &&
				!playerCharacter.IsDashing() &&
				!playerCharacter.IsDashed()) //todo is not collided
			{
				if(reactor < PlayerCharacter::StompThreshold)
				{
					//Start stomping
					playerCharacter.dashDownTimer.Reset();
					playerCharacter.dashPrepTimer.Stop();
				}
				else
				{
					// stomp prepping
					if(playerPhysic.priority < PlayerCharacter::DashPrepPriority)
					{
						playerPhysic.totalForce = ((-body.velocity)/fixedDeltaTime-physicsWorld.gravity())/body.inverseMass;
						playerPhysic.priority = PlayerCharacter::DashPrepPriority;
					}
				}
			}
		}

		//Water gun update
		if(playerCharacter.reserveWaterTimer.Over()) //we emptied the gun tank
		{
			playerCharacter.reloadTimer.Reset(); // reloading start
			playerCharacter.reserveWaterTimer.SetTime({});
		}
		if(playerCharacter.IsReloading())
		{
			target = {};
			playerCharacter.reloadTimer.Update(fixedDeltaTime);
		}
		bool isShooting = target.Length() > neko::Scalar {PlayerCharacter::deadZone};
		if(!isShooting)
		{
			const auto reserveDt = fixedDeltaTime * playerCharacter.reserveWaterTimer.GetPeriod() /
								   playerCharacter.reloadTimer.GetPeriod();
			playerCharacter.reserveWaterTimer.SetTime(playerCharacter.reserveWaterTimer.RemainingTime()+reserveDt);
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
			if(isShooting)
			{
				//TODO not shooting if stomp prep or dashing
				//Shoot wata bullet
				auto& bulletManager = gameSystems_->GetBulletManager();
				const auto speedFactor = playerCharacter.firstShots>0 ?
					neko::Scalar {1.0f} :
					playerCharacter.reserveWaterTimer.RemainingTime();
				bulletManager.SpawnWata(
					body.position+PlayerCharacter::WataOffsetPos+target,
					target.Normalized(),
					playerNumber,
					playerCharacter.firstShots>0,
					speedFactor);
				playerCharacter.waterTimer.Reset();
				if(playerCharacter.firstShots > 0)
				{
					playerCharacter.firstShots--;
				}
			}
		}
		// cap velocity
		if(playerPhysic.priority < PlayerCharacter::CapVelPriority)
		{
			neko::Vec2f wantedVel = body.velocity;
			if(neko::Abs(body.velocity.x) > PlayerCharacter::MaxSpeed)
			{
				wantedVel.x = PlayerCharacter::MaxSpeed * neko::Sign(body.velocity.x);
			}
			if(neko::Abs(body.velocity.y) > PlayerCharacter::MaxSpeed)
			{
				if(!(playerCharacter.IsDashed() || playerCharacter.IsDashing()) &&
					playerCharacter.slowDashTimer.Over())
				{
					wantedVel.y = PlayerCharacter::MaxSpeed * neko::Sign(body.velocity.y);
				}
			}
			if(wantedVel != body.velocity)
			{
				playerPhysic.totalForce = (wantedVel-body.velocity)*body.inverseMass/fixedDeltaTime
					-physicsWorld.gravity()/body.inverseMass;
			}
		}
		// In the end, apply force to physics
		body.force = playerPhysic.totalForce;
		playerPhysic.totalForce = {};
		playerPhysic.priority = 0;
	}

	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		if(!IsValid(playerNumber))
		{
			continue;
		}
		previousPlayerInputs_[playerNumber] = playerInputs_[playerNumber];
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


void PlayerManager::SetPreviousPlayerInput(neko::Span<PlayerInput> playerInputs)
{
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		previousPlayerInputs_[i] = playerInputs[i];
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

	if(otherUserData->type == ColliderType::GAME_LIMIT)
	{
		Respawn(playerNumber);
	}

	if(otherUserData->type == ColliderType::BULLET)
	{
		//TODO manage wata hit
	}
	if(otherUserData->type == ColliderType::PLAYER)
	{
		if(playerIndex == playerPhysics_[playerNumber].headColliderIndex)
		{
			if(otherCollider.colliderIndex == playerPhysics_[otherUserData->playerNumber].footColliderIndex && playerCharacters_[otherUserData->playerNumber].IsDashing())
			{
				//todo dashed
			}
		}
		//dashing on someone head
		if(playerIndex == playerPhysics_[playerNumber].footColliderIndex && playerCharacters_[playerNumber].IsDashing())
		{
			if(otherCollider.colliderIndex == playerPhysics_[otherUserData->playerNumber].headColliderIndex)
			{
				//todo bounce?
			}
		}
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

uint32_t PlayerManager::CalculateChecksum() const
{
	//TODO calculate player character checksum
	std::uint32_t result = 0;
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		const auto* player = reinterpret_cast<const std::uint32_t*>(&playerCharacters_[playerNumber]);
		for(int i = 0; i < sizeof(PlayerCharacter)/sizeof(std::uint32_t); i++)
		{
			result += player[i];
		}
	}

	const auto* playerInput = reinterpret_cast<const std::uint8_t*>(playerInputs_.data());
	for(int i = 0; i < sizeof(playerInputs_); i++)
	{
		result += (std::uint32_t )playerInput[i];
	}
	playerInput = reinterpret_cast<const std::uint8_t*>(previousPlayerInputs_.data());
	for(int i = 0; i < sizeof(playerInputs_); i++)
	{
		result += (std::uint32_t )playerInput[i];
	}
	return result;
}

void PlayerManager::RollbackFrom(const PlayerManager& system)
{
	playerInputs_ = system.playerInputs_;
	previousPlayerInputs_ = system.previousPlayerInputs_;
	playerCharacters_ = system.playerCharacters_;
	playerPhysics_ = system.playerPhysics_;
}

}