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

		const auto reactor = neko::Scalar {playerInput.moveDirY};
		const auto moveX = neko::Abs(playerInput.moveDirX) > PlayerCharacter::deadZone ? neko::Scalar{playerInput.moveDirX} : neko::Scalar{};
		auto target = neko::Vec2f{neko::Scalar {playerInput.targetDirX}, neko::Scalar {playerInput.targetDirY}};

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
				if(newCap > neko::Fixed16{} && neko::Abs(f) > newCap)
				{
					f = newCap * neko::Sign(deltaSpeed) ;
				}
				playerPhysic.AddForce(neko::Vec2f{f, {}}, PlayerCharacter::MovePriority);

			}

		}
		// In Air Move and not dashing!!!
		if(!playerCharacter.IsGrounded() &&
			neko::Abs(moveX) > neko::Scalar{PlayerCharacter::deadZone} &&
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
		if((reactor < PlayerCharacter::ReactorThreshold || body.velocity.y < neko::Fixed{0.0f}) &&
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
				const auto forceCoefficient = neko::Scalar{-0.625f}*playerCharacter.hitTimer.CurrentTime()/PlayerCharacter::HitEffectPeriod+neko::Scalar{1};
				neko::Vec2f newForce = playerCharacter.hitDirection * waterForce * (neko::Scalar)playerCharacter.resistancePhase * forceCoefficient;
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

	if(otherUserData->type == ColliderType::BULLET && otherUserData->playerNumber != playerNumber)
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
		if(playerIndex == playerPhysics_[playerNumber].headColliderIndex)
		{
			if(otherCollider.colliderIndex == playerPhysics_[otherPlayerNumber].footColliderIndex && playerCharacters_[otherPlayerNumber].IsDashing())
			{
				//Dashed collision
				playerCharacters_[playerNumber].dashedTimer.Reset();
				playerCharacters_[playerNumber].collidedPlayer = otherUserData->playerNumber;
				const auto dashedVel = neko::Vec2f(otherBody.velocity.x, PlayerCharacter::DashedSpeed);
				playerPhysics_[playerNumber].AddForce((dashedVel-body.velocity)/body.inverseMass/fixedDeltaTime, PlayerCharacter::DashedPriority);

			}
		}
		//dashing on someone head
		if(playerIndex == playerPhysics_[playerNumber].footColliderIndex && playerCharacters_[playerNumber].IsDashing())
		{
			if(otherCollider.colliderIndex == playerPhysics_[otherPlayerNumber].headColliderIndex)
			{
				if(!playerCharacters_[playerNumber].slowDashTimer.Over())
				{
					playerCharacters_[playerNumber].slowDashTimer.Stop();
					playerCharacters_[playerNumber].bounceDashTimer.Reset();
				}
				if(playerCharacters_[otherPlayerNumber].IsGrounded())
				{
					//Stop dash
					StopDash(playerNumber, DashFinishType::BOUNCE);
				}
				else
				{
					const auto wantedVel = neko::Vec2f{body.velocity.x, {}};
					playerPhysics_[playerNumber].AddForce((wantedVel-body.velocity)/fixedDeltaTime/body.inverseMass, 2);
					//todo recoil timer?
				}
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

uint32_t PlayerManager::CalculateChecksum() const
{
	std::uint32_t result = 0;
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		if(!IsValid(playerNumber))
		{
			continue;
		}
		const auto* player = reinterpret_cast<const std::uint32_t*>(&playerCharacters_[playerNumber]);
		for(std::size_t i = 0; i < sizeof(PlayerCharacter)/sizeof(std::uint32_t); i++)
		{
			result += player[i];
		}
	}

	const auto* playerInput = reinterpret_cast<const std::uint8_t*>(playerInputs_.data());
	for(std::size_t i = 0; i < sizeof(playerInputs_); i++)
	{
		result += (std::uint32_t )playerInput[i];
	}
	playerInput = reinterpret_cast<const std::uint8_t*>(previousPlayerInputs_.data());
	for(std::size_t i = 0; i < sizeof(previousPlayerInputs_); i++)
	{
		result += (std::uint32_t )playerInput[i];
	}
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		if(!IsValid(playerNumber))
		{
			continue;
		}
		const auto& body = gameSystems_->GetPhysicsWorld().body(playerPhysics_[playerNumber].bodyIndex);
		auto* bodyPtr = reinterpret_cast<const std::uint32_t *>(&body);

		for(std::size_t i = 0; i < sizeof(neko::Body)/sizeof(std::uint32_t); i++)
		{
			result += bodyPtr[i];
		}
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