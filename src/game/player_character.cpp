//
// Created by unite on 06.07.2024.
//

#include "game/player_character.h"
#include "game/game_systems.h"
#include "game/const.h"
#include "utils/log.h"
#include "rollback/rollback_manager.h"
#include "utils/adler32.h"

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

		playerPhysic.leftColliderIndex = physicsWorld.AddAabbCollider(playerPhysic.bodyIndex);
		auto& leftCollider = physicsWorld.collider(playerPhysic.leftColliderIndex);
		leftCollider.isTrigger = true;
		leftCollider.offset = PlayerPhysic::leftBox.position+PlayerPhysic::leftBox.offset;
		leftCollider.userData = &playerPhysic.userData;
		auto& leftBox = physicsWorld.aabb(leftCollider.shapeIndex);
		leftBox.halfSize = PlayerPhysic::leftBox.size/neko::Scalar{2};

		playerPhysic.rightColliderIndex = physicsWorld.AddAabbCollider(playerPhysic.bodyIndex);
		auto& rightCollider = physicsWorld.collider(playerPhysic.rightColliderIndex);
		rightCollider.isTrigger = true;
		rightCollider.offset = PlayerPhysic::rightBox.position+PlayerPhysic::rightBox.offset;
		rightCollider.userData = &playerPhysic.userData;
		auto& rightBox = physicsWorld.aabb(rightCollider.shapeIndex);
		rightBox.halfSize = PlayerPhysic::rightBox.size/neko::Scalar{2};

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

		const auto reactor = neko::Scalar {static_cast<float>(playerInput.moveDirY)};
		const auto moveX = neko::Abs(playerInput.moveDirX) > PlayerCharacter::deadZone ? neko::Scalar{static_cast<float>(playerInput.moveDirX)} : neko::Scalar{};
		auto target = neko::Vec2f{neko::Scalar {static_cast<float>(playerInput.targetDirX)}, neko::Scalar {static_cast<float>(playerInput.targetDirY)}};


		playerCharacter.jumpTimer.Update(fixedDeltaTime);
		playerCharacter.collidedTimer.Update(fixedDeltaTime);
		playerCharacter.stopDashTimer.Update(fixedDeltaTime);
		playerCharacter.wasDownRecoverTimer.Update(fixedDeltaTime);

		playerCharacter.dashedTimer.Update(fixedDeltaTime);
		if(!playerCharacter.dashedTimer.Over())
		{
			const auto dashedVel = neko::Vec2f{body.velocity.x, PlayerCharacter::DashedSpeed};
			playerPhysic.AddForce((dashedVel-body.velocity)/fixedDeltaTime/body.inverseMass, PlayerCharacter::DashedPriority);
		}

		if(!playerCharacter.recoilTimer.Over())
		{
			playerCharacter.recoilTimer.Update(fixedDeltaTime);
			const auto forceCoeff = PlayerCharacter::decreaseFactor*playerCharacter.recoilTimer.CurrentRatio()+neko::Scalar{1};
			playerPhysic.AddForce(PlayerCharacter::AttackForce*forceCoeff*playerCharacter.recoilDirection, 1);
		}

		if(!playerCharacter.respawnStaticTime.Over())
		{
			playerCharacter.respawnStaticTime.Update(fixedDeltaTime);
			if(playerCharacter.respawnStaticTime.Over() ||
				neko::Abs(playerInput.moveDirX) > PlayerCharacter::deadZone ||
				neko::Abs(playerInput.moveDirY) > PlayerCharacter::deadZone ||
				reactor > PlayerCharacter::ReactorThreshold)
			{
				body.position = spawnPositions[playerNumber];
				body.isActive = true;
				playerCharacter.respawnStaticTime.Stop();
				playerCharacter.invincibleTimer.Reset();
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

		if(!playerCharacter.invincibleTimer.Over())
		{
			playerCharacter.invincibleTimer.Update(fixedDeltaTime);
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
			playerCharacter.dashedTimer.Stop();

			if(reactor > PlayerCharacter::StompThreshold)
			{
				playerCharacter.stopDashTimer.Stop();
			}
			//on ground
			const auto wantedSpeed = moveX * PlayerCharacter::WalkSpeed;
			const auto velX = body.velocity.x;
			const auto deltaSpeed = wantedSpeed-velX;
			auto newCap = PlayerCharacter::CapMoveForce;

			if(playerPhysic.GetPriority() <= PlayerCharacter::MovePriority)
			{
				if(playerPhysic.GetPriority() < PlayerCharacter::MovePriority)
				{
					//Counter gravity
					//playerPhysic.AddForce( -physicsWorld.gravity()/body.inverseMass, PlayerCharacter::MovePriority);

				}

				auto f = deltaSpeed / fixedDeltaTime / body.inverseMass;

				if(playerCharacter.resistancePhase >= 2)
				{
					const auto divisor = neko::Exp<neko::Scalar>()*neko::Scalar(playerCharacter.resistancePhase-1);
					newCap = (PlayerCharacter::CapMoveForce-PlayerCharacter::WetCapMoveForce)/divisor+PlayerCharacter::WetCapMoveForce;
				}
				if(newCap > neko::Scalar{} && neko::Abs(f) > newCap)
				{
					f = newCap * neko::Sign(deltaSpeed) ;
				}
				playerPhysic.AddForce(neko::Vec2f{f, {}}, PlayerCharacter::MovePriority);

			}

		}
		// In Air Move and not dashing!!!
		if(!playerCharacter.IsGrounded() &&
			neko::Abs(moveX) > neko::Scalar{static_cast<float>(PlayerCharacter::deadZone)} &&
			!playerCharacter.IsDashing())
		{
			const auto horizontalForce = PlayerCharacter::InAirForce * moveX;
			playerPhysic.AddForce(neko::Vec2f{horizontalForce, {}}, PlayerCharacter::MovePriority);
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
		if((playerCharacter.IsGrounded() && (reactor > PlayerCharacter::GroundReactorThreshold || !playerCharacter.jumpTimer.Over())) ||
			(!playerCharacter.IsGrounded() && reactor > PlayerCharacter::ReactorThreshold) ||
			(!playerCharacter.IsGrounded() && !playerCharacter.jumpTimer.Over() && playerCharacter.jetBurstCoolDownTimer.Over()))
		{
			const auto velY = body.velocity.y;
			const auto decreaseFactor = velY > neko::Scalar{} ? neko::Scalar{ 0.75f } : neko::Scalar{ 1.0f };
			auto force = PlayerCharacter::ReactorForce * reactor * decreaseFactor;
			if(playerCharacter.jetBurstCoolDownTimer.Over() &&
				!jetbursting &&
				reactor > PlayerCharacter::JetBurstThreshold &&
				playerCharacter.collidedTimer.Over())
			{
				jetbursting = true;
				playerCharacter.preJetBurstTimer.Reset();
				playerCharacter.jetBurstCoolDownTimer.Reset();
			}
			if(jetbursting)
			{
				force = PlayerCharacter::ReactorForce;
			}
			const auto jumpSpeed = PlayerCharacter::JumpForce*body.inverseMass*fixedDeltaTime;
			if((playerCharacter.IsGrounded() || (jetbursting && playerCharacter.preJetBurstTimer.Over())) &&
				playerCharacter.jumpTimer.Over() &&
				velY < jumpSpeed &&
				body.velocity.Length() < PlayerCharacter::StompOrBurstMaxVelocity &&
				playerCharacter.stopDashTimer.Over() &&
				playerCharacter.collidedTimer.Over() &&
				!playerCharacter.IsDashed())
			{
				//Doing a jet burst

				force = (jumpSpeed-body.velocity.y) / fixedDeltaTime / body.inverseMass;
				playerCharacter.jumpTimer.Reset();
			}

			if(!playerCharacter.jumpTimer.Over())
			{
				const auto t = neko::Scalar {1}-playerCharacter.jumpTimer.CurrentRatio();
				force *= (t*t*t*t);
			}

			if(!playerCharacter.collidedTimer.Over() && !playerCharacter.IsGrounded())
			{
				force = {};
			}
			playerPhysic.AddForce( neko::Vec2f{{}, force}, PlayerCharacter::JetPackPriority);
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
		if((reactor < PlayerCharacter::ReactorThreshold || body.velocity.y < neko::Scalar{0.0f}) &&
			playerCharacter.jumpTimer.RemainingTime() < PlayerCharacter::JumpCancelTime)
		{
			playerCharacter.jumpTimer.Stop();
		}

		//stomp update
		if(!playerCharacter.dashPrepTimer.Over())
		{
			playerCharacter.dashPrepTimer.Update(fixedDeltaTime);
			if(playerCharacter.dashPrepTimer.Over())
			{
				playerCharacter.wasDownRecoverTimer.Reset();
			}
		}

		if(!playerCharacter.dashDownTimer.Over())
		{
			playerCharacter.dashDownTimer.Update(fixedDeltaTime);
			if(playerCharacter.dashDownTimer.Over())
			{
				//Finish stomp
				StopDash(playerNumber, DashFinishType::SLOW);
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
				playerPhysic.AddForce(neko::Vec2f{{}, PlayerCharacter::SlowDashForce}, PlayerCharacter::SlowDashPriority);
			}
		}
		if(!playerCharacter.bounceDashTimer.Over())
		{
			if(!playerCharacter.IsGrounded())
			{
				playerPhysic.AddForce(neko::Vec2f {{}, PlayerCharacter::SlowDashForce}, PlayerCharacter::SlowDashPriority);
				playerCharacter.bounceDashTimer.Update(fixedDeltaTime);
			}
			else
			{
				playerCharacter.bounceDashTimer.Stop();
			}
		}
		if(!playerCharacter.IsGrounded())
		{
			if(!playerCharacter.IsDashing() &&
				!playerCharacter.IsDashPrepping() &&
				reactor < PlayerCharacter::FallingThreshold &&
				playerCharacter.stopDashTimer.Over())
			{
				playerPhysic.AddForce(neko::Vec2f({}, PlayerCharacter::FallingForce * reactor), PlayerCharacter::FallingPriority);
			}
			if(playerInput.GetStomp() &&
				!previousPlayerInputs_[playerNumber].GetStomp() &&
				!playerCharacter.IsDashing() &&
				!playerCharacter.IsDashPrepping() &&
				!playerCharacter.IsDashed() &&
				body.velocity.Length() < PlayerCharacter::StompOrBurstMaxVelocity &&
				playerCharacter.collidedTimer.Over() &&
				playerCharacter.wasDownRecoverTimer.Over())
			{
				//Start stomp prep
				playerCharacter.dashPrepTimer.Reset();
			}
			else if(playerCharacter.IsDashing() && playerCharacter.stopDashTimer.Over())
			{
				//Update stomp with moveX
				neko::Vec2f vel{moveX*PlayerCharacter::DashSpeed, -PlayerCharacter::DashSpeed};
				playerPhysic.AddForce((vel-body.velocity)/fixedDeltaTime/body.inverseMass, PlayerCharacter::DashPriority);

				if(playerCharacter.dashPositions.is_full())
				{
					playerCharacter.dashPositions.erase(playerCharacter.dashPositions.cend()-1);
				}
				playerCharacter.dashPositions.insert(playerCharacter.dashPositions.cbegin(), body.position);
			}
			else if(playerCharacter.IsDashPrepping() &&
				!playerCharacter.IsDashing() &&
				!playerCharacter.IsDashed() &&
				playerCharacter.collidedTimer.Over() &&
				playerCharacter.stopDashTimer.Over())
			{
				if(reactor < PlayerCharacter::StompThreshold)
				{
					//Start stomping
					playerCharacter.dashDownTimer.Reset();
					playerCharacter.dashPrepTimer.Stop();
					playerCharacter.dashPositions.clear();

				}
				else
				{
					// stomp prepping
					playerPhysic.AddForce(((-body.velocity)/fixedDeltaTime-physicsWorld.gravity())/body.inverseMass, PlayerCharacter::DashPrepPriority);
					//todo add was down recover timer to forbid unlimited use of stomp
				}
			}
			else if(reactor >= PlayerCharacter::StompThreshold)
			{
				playerCharacter.dashDownTimer.Stop();
			}
		}
		else
		{
			playerCharacter.dashDownTimer.Stop();
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
		bool isShooting = target.Length() > neko::Scalar {static_cast<float>(PlayerCharacter::deadZone)};
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
			if(isShooting && !playerCharacter.IsDashPrepping() && !playerCharacter.IsDashing())
			{
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
		// wata hit
		if(!playerCharacter.hitTimer.Over())
		{
			if(playerCharacter.hitTimer.CurrentTime() < PlayerCharacter::HitEffectPeriod)
			{
				if(playerCharacter.IsGrounded() &&
				playerCharacter.hitDirection.y < neko::Scalar{0.1f} &&
				playerCharacter.resistancePhase == PlayerCharacter::MaxResistancePhase)
				{
					if (playerCharacter.hitDirection.y < neko::Scalar{ 0 })
					{
						playerCharacter.hitDirection = neko::Vec2f {playerCharacter.hitDirection.x,-playerCharacter.hitDirection.y * neko::Scalar{0.9f}}.Normalized();
					}
					else
					{
						playerCharacter.hitDirection = neko::Vec2f {playerCharacter.hitDirection.x,neko::Scalar {0.2f}}.Normalized();
					}
				}

				neko::Scalar waterForce = PlayerCharacter::WaterForce;
				const auto forceCoefficient = PlayerCharacter::decreaseFactor*playerCharacter.hitTimer.CurrentTime()/PlayerCharacter::HitEffectPeriod+neko::Scalar{1};
				neko::Vec2f newForce = playerCharacter.hitDirection * waterForce * static_cast<neko::Scalar>(playerCharacter.resistancePhase) * forceCoefficient;
				playerPhysic.AddForce(newForce, PlayerCharacter::HitPriority);
			}
			playerCharacter.hitTimer.Update(fixedDeltaTime);
		}
		else
		{
			playerCharacter.resistancePhase = 1;
			playerCharacter.hitPlayer = -1;
		}
		// cap velocity
		if(playerPhysic.GetPriority() < PlayerCharacter::CapVelPriority)
		{
			neko::Vec2f wantedVel = body.velocity;
			if(neko::Abs(body.velocity.x) > PlayerCharacter::MaxSpeed)
			{
				wantedVel.x = PlayerCharacter::MaxSpeed * neko::Sign(body.velocity.x);
			}
			if(neko::Abs(body.velocity.y) > PlayerCharacter::MaxSpeed)
			{
				if(!(playerCharacter.IsDashed() || playerCharacter.IsDashing()) &&
					playerCharacter.slowDashTimer.Over() &&
					playerCharacter.stopDashTimer.Over())
				{
					wantedVel.y = PlayerCharacter::MaxSpeed * neko::Sign(body.velocity.y);
				}
			}
			if(wantedVel != body.velocity)
			{
				playerPhysic.AddForce((wantedVel-body.velocity)/fixedDeltaTime/body.inverseMass
									  -physicsWorld.gravity()/body.inverseMass + playerPhysic.GetForce(), PlayerCharacter::CapVelPriority);
			}
		}
		// In the end, apply force to physics
		body.force = playerPhysic.GetForce();
		playerPhysic.Reset();
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

	const auto& body = gameSystems_->GetPhysicsWorld().body(playerPhysics_[playerNumber].bodyIndex);
	const auto& otherBody = gameSystems_->GetPhysicsWorld().body(otherCollider.bodyIndex);
	const auto* otherUserData = static_cast<const ColliderUserData*>(otherCollider.userData);
	if(playerIndex == playerPhysics_[playerNumber].footColliderIndex)
	{
		if(otherUserData->type == ColliderType::PLATFORM)
		{
			playerCharacters_[playerNumber].footCount++;
			if(!playerCharacters_[playerNumber].slowDashTimer.Over())
			{
				playerCharacters_[playerNumber].slowDashTimer.Stop();
				playerCharacters_[playerNumber].bounceDashTimer.Reset();
			}
		}

	}

	if(otherUserData->type == ColliderType::GAME_LIMIT)
	{
		if(!playerCharacters_[playerNumber].collidedTimer.Over())
		{
			playerCharacters_[playerCharacters_[playerNumber].collidedPlayer].killCount++;
		}
		if(!playerCharacters_[playerNumber].hitTimer.Over())
		{
			playerCharacters_[playerCharacters_[playerNumber].hitPlayer].killCount++;
		}
		if(playerCharacters_[playerNumber].IsDashed())
		{
			playerCharacters_[playerCharacters_[playerNumber].collidedPlayer].killCount++;
		}
		playerCharacters_[playerNumber].fallCount++;
		Respawn(playerNumber);
	}

	if(otherUserData->type == ColliderType::BULLET &&
		otherUserData->playerNumber != playerNumber && !playerCharacters_[playerNumber].IsInvincible())
	{
		playerCharacters_[playerNumber].hitDirection = otherBody.velocity.Normalized();
		if(!playerCharacters_[playerNumber].hitTimer.Over())
		{
			playerCharacters_[playerNumber].resistancePhase++;
			if (playerCharacters_[playerNumber].resistancePhase >= PlayerCharacter::MaxResistancePhase)
			{
				playerCharacters_[playerNumber].resistancePhase = PlayerCharacter::MaxResistancePhase;
			}
		}
		playerCharacters_[playerNumber].hitTimer.Reset();
		playerCharacters_[playerNumber].hitPlayer = otherUserData->playerNumber;
	}
	if(otherUserData->type == ColliderType::PLAYER)
	{
		const auto otherPlayerNumber = otherUserData->playerNumber;
		if(playerIndex == playerPhysics_[playerNumber].headColliderIndex &&
			otherCollider.colliderIndex == playerPhysics_[otherPlayerNumber].footColliderIndex &&
			playerCharacters_[otherPlayerNumber].IsDashing())
		{
			if(!playerCharacters_[playerNumber].IsInvincible())
			{
				//LogDebug(fmt::format("Dashed player {} on player {}", playerNumber+1, otherPlayerNumber+1));
				//Dashed collision
				playerCharacters_[playerNumber].dashedTimer.Reset();
				playerCharacters_[playerNumber].collidedPlayer = otherUserData->playerNumber;
				if(!playerCharacters_[playerNumber].IsGrounded())
				{
					const auto dashedVel = neko::Vec2f(otherBody.velocity.x, PlayerCharacter::DashedSpeed);
					playerPhysics_[playerNumber].AddForce((dashedVel - body.velocity) / body.inverseMass / GetFixedDeltaTime(),
						PlayerCharacter::DashedPriority);
				}
			}

			//LogDebug(fmt::format("Dashing player {} on player {}", otherPlayerNumber+1, playerNumber+1));
			if(!playerCharacters_[otherPlayerNumber].slowDashTimer.Over())
			{
				playerCharacters_[otherPlayerNumber].slowDashTimer.Stop();
				playerCharacters_[otherPlayerNumber].bounceDashTimer.Reset();
			}
			if(playerCharacters_[playerNumber].IsGrounded())
			{
				//Stop dash
				StopDash(otherPlayerNumber, DashFinishType::BOUNCE);
			}
			else
			{
				const auto wantedVel = neko::Vec2f{otherBody.velocity.x, {}};
				playerPhysics_[otherPlayerNumber].AddForce((wantedVel-otherBody.velocity)/fixedDeltaTime/otherBody.inverseMass, 2);
				playerCharacters_[otherPlayerNumber].recoilDirection = -otherBody.velocity.Normalized();
				playerCharacters_[otherPlayerNumber].recoilTimer.Reset();
			}

		}
		//Simple side collision
		if(playerIndex == playerPhysics_[playerNumber].rightColliderIndex || playerIndex == playerPhysics_[playerNumber].leftColliderIndex)
		{
			const auto otherColliderIndex = otherCollider.colliderIndex;
			if(otherColliderIndex == playerPhysics_[otherPlayerNumber].rightColliderIndex || otherColliderIndex == playerPhysics_[otherPlayerNumber].leftColliderIndex)
			{
				if(playerCharacters_[otherPlayerNumber].IsCollided() && playerCharacters_[otherPlayerNumber].collidedPlayer == playerNumber)
				{
					return;
				}
				//velocity collision
				//Exchange velocities

				const auto playerVel = body.velocity;
				const auto otherVel = otherBody.velocity;

				if(playerCharacters_[playerNumber].IsDashed())
				{
					//Spread dash
					//other player get dashed velocity priority 3
					playerPhysics_[otherPlayerNumber].AddForce((neko::Vec2f(body.velocity.x, PlayerCharacter::DashedSpeed)-otherBody.velocity)
															   *otherBody.inverseMass/fixedDeltaTime, 3);

				}

				if(playerCharacters_[otherPlayerNumber].IsDashed())
				{
					//Spread dash
					//player get dashed velocity priority 3
					playerPhysics_[playerNumber].AddForce((neko::Vec2f(otherBody.velocity.x, PlayerCharacter::DashedSpeed)-body.velocity)
														  *body.inverseMass/fixedDeltaTime, 3);
				}


				playerPhysics_[playerNumber].AddForce((otherVel-playerVel)*body.inverseMass/fixedDeltaTime, 2);
				playerPhysics_[otherPlayerNumber].AddForce((playerVel-otherVel)*body.inverseMass/fixedDeltaTime, 2);
				playerCharacters_[playerNumber].collidedTimer.Reset();
				playerCharacters_[playerNumber].collidedPlayer = otherPlayerNumber;
				playerCharacters_[otherPlayerNumber].collidedTimer.Reset();
				playerCharacters_[otherPlayerNumber].collidedPlayer = playerNumber;

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

Checksum<static_cast<int>(PlayerChecksumIndex::LENGTH)> PlayerManager::CalculateChecksum() const
{
	Adler32 playerCharacterResult{};
	Adler32 playerPhysicsResult{};
	Adler32 playerBodyResult{};
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		if(!IsValid(playerNumber))
		{
			continue;
		}

		const auto& playerCharacter = playerCharacters_[playerNumber];
		playerCharacterResult.Add(playerCharacter.respawnPauseTimer);
		playerCharacterResult.Add(playerCharacter.respawnMoveTimer);
		playerCharacterResult.Add(playerCharacter.respawnStaticTime);
		playerCharacterResult.Add(playerCharacter.invincibleTimer);
		playerCharacterResult.Add(playerCharacter.hitTimer);
		playerCharacterResult.Add(playerCharacter.reserveWaterTimer);
		playerCharacterResult.Add(playerCharacter.waterTimer);
		playerCharacterResult.Add(playerCharacter.reloadTimer);
		playerCharacterResult.Add(playerCharacter.jetBurstCoolDownTimer);
		playerCharacterResult.Add(playerCharacter.jumpTimer);
		playerCharacterResult.Add(playerCharacter.preJetBurstTimer);
		playerCharacterResult.Add(playerCharacter.dashDownTimer);
		playerCharacterResult.Add(playerCharacter.stopDashTimer);
		playerCharacterResult.Add(playerCharacter.slowDashTimer);
		playerCharacterResult.Add(playerCharacter.bounceDashTimer);
		playerCharacterResult.Add(playerCharacter.dashPrepTimer);
		playerCharacterResult.Add(playerCharacter.dashedTimer);
		playerCharacterResult.Add(playerCharacter.wasDownRecoverTimer);
		playerCharacterResult.Add(playerCharacter.recoilTimer);
		playerCharacterResult.Add(playerCharacter.collidedTimer);
		playerCharacterResult.Add(playerCharacter.hitDirection);
		playerCharacterResult.Add(playerCharacter.recoilDirection);
		playerCharacterResult.Add(playerCharacter.fallCount);
		playerCharacterResult.Add(playerCharacter.killCount);
		playerCharacterResult.Add(playerCharacter.resistancePhase);
		playerCharacterResult.Add(playerCharacter.hitPlayer);
		playerCharacterResult.Add(playerCharacter.firstShots);
		playerCharacterResult.Add(playerCharacter.collidedPlayer);
		playerCharacterResult.Add(playerCharacter.footCount);


		const auto& playerPhysic = playerPhysics_[playerNumber];
		playerPhysicsResult.Add(playerPhysic.bodyIndex);
		playerPhysicsResult.Add(playerPhysic.colliderIndex);
		playerPhysicsResult.Add(playerPhysic.footColliderIndex);
		playerPhysicsResult.Add(playerPhysic.headColliderIndex);
		playerPhysicsResult.Add(playerPhysic.leftColliderIndex);
		playerPhysicsResult.Add(playerPhysic.rightColliderIndex);
		playerPhysicsResult.Add(playerPhysic.userData);
		playerPhysicsResult.Add(playerPhysic.GetForce());
		playerPhysicsResult.Add(playerPhysic.GetPriority());

		const auto& body = gameSystems_->GetPhysicsWorld().body(playerPhysics_[playerNumber].bodyIndex);
		playerBodyResult.Add(body.position);
		playerBodyResult.Add(body.velocity);
		playerBodyResult.Add(body.force);
		playerBodyResult.Add(body.inverseMass);
		playerBodyResult.Add(body.type);
		playerBodyResult.Add(body.isActive);
	}

	Adler32 playerInputResult{};
	playerInputResult.Add(playerInputs_);

	Adler32 previousPlayerInputResult{};
	previousPlayerInputResult.Add(previousPlayerInputs_);

	return {playerCharacterResult.GetValue(), playerPhysicsResult.GetValue(), playerInputResult.GetValue(), previousPlayerInputResult.GetValue(), playerBodyResult.GetValue()};
}

void PlayerManager::RollbackFrom(const PlayerManager& system)
{
	playerInputs_ = system.playerInputs_;
	previousPlayerInputs_ = system.previousPlayerInputs_;
	playerCharacters_ = system.playerCharacters_;
	playerPhysics_ = system.playerPhysics_;
}
void PlayerManager::StopDash(int playerNumber, PlayerManager::DashFinishType bounce)
{
	auto& playerCharacter = playerCharacters_[playerNumber];
	playerCharacter.dashDownTimer.Stop();
	playerCharacter.slowDashTimer.Stop();

	switch(bounce)
	{
	case DashFinishType::BOUNCE:
	{
		playerCharacter.bounceDashTimer.Reset();
		playerCharacter.stopDashTimer.Reset();
		break;
	}
	case DashFinishType::SLOW:
	{
		playerCharacter.slowDashTimer.Reset();
		playerCharacter.jetBurstCoolDownTimer.Stop();
		playerCharacter.stopDashTimer.Reset();
		break;
	}
	default:break;
	}
}

}