#include "game/graphics/player_view.h"
#include "graphics/graphics_manager.h"
#include "utils/log.h"

#include <math/fixed_lut.h>
#include <fmt/format.h>
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

static constexpr std::array<std::string_view, (int)PlayerRenderState::LENGTH> playerAnimNames
	{{
		"idle",
		"walk",
		"walkback",
		"shoot",
		"jet",
		"jetburst",
		"fall",
		"dashprep",
		"dash",
		"bounce"
	}};
static constexpr std::array<std::string_view, MaxPlayerNmb> cloudSkinNames{{
	"P1_cyan",
	"P2_orange",
	"P3_magenta",
	"P4_turquoise"
}};
void PlayerView::Begin()
{

}
void PlayerView::End()
{

}
void PlayerView::Update([[maybe_unused]]float dt)
{
	if (!IsSpineLoaded())
		return;
	if(playerRenderDatas_[0].bodyDrawable == nullptr)
	{
		Load();
		return;
	}
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	const auto& playerManager = gameSystems_->GetPlayerManager();

	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		const auto& playerCharacter = playerManager.GetPlayerCharacter()[playerNumber];
		const auto& playerInput = playerManager.GetPlayerInputs()[playerNumber];
		auto& playerRenderData = playerRenderDatas_[playerNumber];
		playerRenderData.bodyDrawable->skeleton->setToSetupPose();

		if(!playerRenderData.isRespawning)
		{
			const auto targetDir = neko::Vec2f((neko::Scalar)playerInput.targetDirX, (neko::Scalar)playerInput.targetDirY);
			const bool isShooting = targetDir.Length() > (neko::Scalar)PlayerCharacter::deadZone && !playerCharacter.IsReloading();
			if(isShooting)
			{
				if ((playerInput.targetDirX > PlayerCharacter::deadZone && !playerRenderData.faceRight) ||
					(playerInput.targetDirX < -PlayerCharacter::deadZone && playerRenderData.faceRight))
				{
					playerRenderData.faceRight = !playerRenderData.faceRight;
				}
			}
			else
			{
				if ((playerInput.moveDirX > PlayerCharacter::deadZone && !playerRenderData.faceRight) ||
					(playerInput.moveDirX < -PlayerCharacter::deadZone && playerRenderData.faceRight))
				{
					playerRenderData.faceRight = !playerRenderData.faceRight;
				}
			}
			if(!playerCharacter.respawnStaticTime.Over() ||
				!playerCharacter.respawnMoveTimer.Over() ||
				!playerCharacter.respawnPauseTimer.Over())
			{
				playerRenderData.isRespawning = true;
				playerRenderData.cloudDrawable->animationState->setAnimation(0, "respawn", true);
				SwitchToState(PlayerRenderState::IDLE, playerNumber);
				playerRenderData.cloudDrawable->update(dt, spine::Physics_Update);
				continue;
			}

			if(playerCharacter.IsDashPrepping())
			{
				SwitchToState(PlayerRenderState::DASHPREP, playerNumber);
			}
			if(playerCharacter.IsDashing())
			{
				SwitchToState(PlayerRenderState::DASH, playerNumber);
			}
			switch (playerRenderData.state)
			{

			case PlayerRenderState::IDLE:
			{
				if (!playerCharacter.IsGrounded())
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
					break;
				}
				if (neko::Abs<neko::Fixed8>(playerInput.moveDirX) > PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::WALK, playerNumber);
				}
				else
				{
					if (isShooting)
					{
						SwitchToState(PlayerRenderState::SHOOT, playerNumber);
					}
				}
				break;
			}
			case PlayerRenderState::WALK:
			{
				if (!playerCharacter.IsGrounded())
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				if (neko::Abs(playerInput.moveDirX) < PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				else
				{
					if ((playerInput.moveDirX > neko::Fixed8{0.0f} && playerInput.targetDirX < neko::Fixed8{ 0.0f }) ||
						(playerInput.moveDirX < neko::Fixed8{0.0f} && playerInput.targetDirX > neko::Fixed8{ 0.0f }))
					{
						SwitchToState(PlayerRenderState::WALK_BACK, playerNumber);
					}
				}
				break;
			}
			case PlayerRenderState::WALK_BACK:
			{
				if (!playerCharacter.IsGrounded())
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				if (neko::Abs<neko::Fixed8>(playerInput.moveDirX) < PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				else
				{
					if ((playerInput.moveDirX < neko::Fixed8{0.0f}  && playerInput.targetDirX < neko::Fixed8{ 0.0f }) ||
						(playerInput.moveDirX > neko::Fixed8{0.0f}  && playerInput.targetDirX > neko::Fixed8{ 0.0f }))
					{
						SwitchToState(PlayerRenderState::WALK, playerNumber);
					}
				}
				break;
			}
			case PlayerRenderState::FALL:
			{
				if (playerCharacter.IsGrounded())
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				else
				{
					if (neko::Scalar{ playerInput.moveDirY } > PlayerCharacter::JetBurstThreshold)
					{
						if(playerCharacter.jumpTimer.Over())
						{
							SwitchToState(PlayerRenderState::JET, playerNumber);
						}
						else
						{
							SwitchToState(PlayerRenderState::JETBURST, playerNumber);
						}
					}
				}


				break;
			}
			case PlayerRenderState::SHOOT:
			{
				if (neko::Abs<neko::Fixed8>(playerInput.moveDirX) > PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::WALK, playerNumber);
				}
				else
				{
					if (!isShooting)
					{
						SwitchToState(PlayerRenderState::IDLE, playerNumber);
					}
					else
					{
						if ((targetDir.x > (neko::Scalar)PlayerCharacter::deadZone && !playerRenderData.faceRight) ||
							(targetDir.x < (neko::Scalar)-PlayerCharacter::deadZone && playerRenderData.faceRight))
						{
							playerRenderData.faceRight = !playerRenderData.faceRight;
						}
					}
				}
				break;
			}
			case PlayerRenderState::JET:
			{
				if (playerCharacter.IsGrounded())
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				if (neko::Scalar{ playerInput.moveDirY } < PlayerCharacter::ReactorThreshold)
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				if(!playerCharacter.jumpTimer.Over())
				{
					SwitchToState(PlayerRenderState::JETBURST, playerNumber);
				}
				break;

			}
			case PlayerRenderState::JETBURST:
			{
				if (playerCharacter.jumpTimer.Over())
				{
					if (neko::Scalar{ playerInput.moveDirY } > PlayerCharacter::JetBurstThreshold)
					{
						SwitchToState(PlayerRenderState::JET, playerNumber);
					}
					else
					{
						SwitchToState(PlayerRenderState::FALL, playerNumber);
					}
				}
				break;
			}
			case PlayerRenderState::DASHPREP:
			{
				if(!playerCharacter.IsDashPrepping())
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				break;
			}
			case PlayerRenderState::DASH:
			{
				if(!playerCharacter.IsDashing())
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				break;
			}
			case PlayerRenderState::BOUNCE:
				break;
			case PlayerRenderState::LENGTH:
				break;
			}

			if(isShooting)
			{

				if(!playerRenderData.wasShooting)
				{
					//Move arm
					playerRenderData.armDrawable->animationState->setAnimation(0, "shoot", false);
					playerRenderData.wasShooting = true;
				}
			}
			else
			{
				if(playerRenderData.wasShooting)
				{
					//Move back arm
					playerRenderData.armDrawable->animationState->setAnimation(0, "put away", false);
					playerRenderData.wasShooting = false;
				}
			}

		}
		else
		{
			//is respawning
			if(playerCharacter.respawnStaticTime.Over() &&
				playerCharacter.respawnMoveTimer.Over() &&
				playerCharacter.respawnPauseTimer.Over())
			{
				playerRenderData.isRespawning = false;
				playerRenderData.cloudEndRespawnTimer.Reset();
				playerRenderData.cloudDrawable->animationState->setAnimation(0, "respawn destroy", false);

				playerRenderData.cloudDrawable->update(dt, spine::Physics_Update);
			}
		}
	}

	UpdateTransforms(dt);


}
void PlayerView::Draw()
{
#ifdef TRACY_ENABLE
 	ZoneScoped;
#endif
	auto* renderer = GetRenderer();
	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	const auto& playerManager = gameSystems_->GetPlayerManager();
	const auto& playerPhysics = playerManager.GetPlayerPhysics();
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		if (!playerRenderDatas_[i].bodyDrawable)
		{
			continue;
		}
		const auto& body = physicsWorld.body(playerPhysics[i].bodyIndex);
		auto& bodyDrawable = playerRenderDatas_[i].bodyDrawable;
		auto& armDrawable = playerRenderDatas_[i].armDrawable;
		auto& gunDrawable = playerRenderDatas_[i].gunDrawable;

		// Draw in correct order
		bodyDrawable->draw(renderer);
		if(playerRenderDatas_[i].state != PlayerRenderState::DASHPREP)
		{
			gunDrawable->draw(renderer);
			armDrawable->draw(renderer);
		}
		if (playerRenderDatas_[i].isRespawning || !playerRenderDatas_[i].cloudEndRespawnTimer.Over())
		{
			auto& cloudDrawable = playerRenderDatas_[i].cloudDrawable;
			cloudDrawable->draw(renderer);
		}

		if (GetDebugConfig().showPhysicsBox)
		{
			const auto rect = GetDrawingRect(
				body.position + PlayerPhysic::box.offset,
				PlayerPhysic::box.size);
			const auto& color = playerColors[i];
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawRect(renderer, &rect);

			const auto footRect =
				GetDrawingRect(body.position + PlayerPhysic::footBox.offset,
					PlayerPhysic::footBox.size);
			SDL_RenderDrawRect(renderer, &footRect);
			const auto headRect =
				GetDrawingRect(body.position + PlayerPhysic::headBox.position+PlayerPhysic::headBox.offset,
					PlayerPhysic::headBox.size);
			SDL_RenderDrawRect(renderer, &headRect);

		}
	}

}

PlayerView::PlayerView(const GameSystems* gameSystems): gameSystems_(gameSystems)
{

}


void PlayerView::SwitchToState(PlayerRenderState state, int playerNumber)
{
	playerRenderDatas_[playerNumber].bodyDrawable->animationState->setAnimation(0,
		playerAnimNames[(int)state].data(), true);
	playerRenderDatas_[playerNumber].state = state;
}
void PlayerView::Load()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		playerRenderDatas_[i].bodyDrawable = CreateSkeletonDrawable((SpineManager::SkeletonId)((int)SpineManager::CAT_NOARM+i));
		playerRenderDatas_[i].bodyDrawable->animationState->setAnimation(0, "idle", true);

		playerRenderDatas_[i].shoulderBone = playerRenderDatas_[i].bodyDrawable->skeleton->findBone("BN_armright");


		playerRenderDatas_[i].armDrawable = CreateSkeletonDrawable((SpineManager::SkeletonId)((int)SpineManager::CAT_ARM+i));
		playerRenderDatas_[i].armDrawable->animationState->setAnimation(0, "idle", true);
		playerRenderDatas_[i].handBone = playerRenderDatas_[i].armDrawable->skeleton->findBone("pistolgrip");

		playerRenderDatas_[i].gunDrawable = CreateSkeletonDrawable(SpineManager::BASEGUN);
		playerRenderDatas_[i].gunDrawable->animationState->setAnimation(0, "idle", true);

		playerRenderDatas_[i].cloudDrawable = CreateSkeletonDrawable(SpineManager::CLOUD);
		playerRenderDatas_[i].cloudDrawable->skeleton->setSkin(cloudSkinNames[i].data());
	}
}
void PlayerView::UpdateTransforms(float dt)
{
	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	const auto& playerManager = gameSystems_->GetPlayerManager();
	const auto& playerPhysics = playerManager.GetPlayerPhysics();

	const auto& playerInputs = playerManager.GetPlayerInputs();
	const auto scale = playerScale * GetGraphicsScale();
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		auto& playerRenderData = playerRenderDatas_[playerNumber];
		const auto& playerCharacter = playerManager.GetPlayerCharacter()[playerNumber];
		const auto& body = physicsWorld.body(playerPhysics[playerNumber].bodyIndex);
		const auto position = GetGraphicsPosition(body.position);
		if (!playerRenderData.bodyDrawable)
		{
			continue;
		}
		auto& bodyDrawable = playerRenderData.bodyDrawable;
		bodyDrawable->skeleton->setScaleX((playerRenderData.faceRight ? 1.0f : -1.0f) * scale);
		bodyDrawable->skeleton->setScaleY(scale);
		if (!playerRenderData.isRespawning)
		{
			bodyDrawable->skeleton->setPosition((float)position.x, (float)position.y);
		}
		else
		{
			if (!playerCharacter.respawnPauseTimer.Over())
			{
				bodyDrawable->skeleton->setPosition((float)position.x, (float)position.y);
			}
			else if (!playerCharacter.respawnMoveTimer.Over())
			{

				const auto spawnPosition = neko::Vec2<float>(GetGraphicsPosition(PlayerManager::spawnPositions[playerNumber]));
				const auto physicsPosition = neko::Vec2<float>(position);
				const auto midPosition = physicsPosition + (spawnPosition - physicsPosition)
														   * (float)playerCharacter.respawnMoveTimer.CurrentRatio();
				bodyDrawable->skeleton->setPosition(midPosition.x, midPosition.y);
			}
			else if (!playerCharacter.respawnStaticTime.Over())
			{
				const auto spawnPosition = neko::Vec2<float>(GetGraphicsPosition(PlayerManager::spawnPositions[playerNumber]));
				bodyDrawable->skeleton->setPosition(spawnPosition.x, spawnPosition.y);
			}
		}
		float animRatio = 1.0f;
		if (playerRenderData.state == PlayerRenderState::WALK || playerRenderData.state == PlayerRenderState::WALK_BACK)
		{
			animRatio = (float)neko::Abs(playerInputs[playerNumber].moveDirX);
		}
		bodyDrawable->update(animRatio * dt, spine::Physics_Update);

		auto& armDrawable = playerRenderData.armDrawable;
		armDrawable->skeleton->setScaleX((playerRenderData.faceRight ? 1.0f : -1.0f) * scale);
		armDrawable->skeleton->setScaleY(scale);
		const auto targetDir = neko::Vec2<float>((float)playerInputs[playerNumber]
			.targetDirX, (float)playerInputs[playerNumber].targetDirY);
		const bool isShooting = targetDir.Length() > (float)PlayerCharacter::deadZone && !playerCharacter.IsReloading();


		auto* shoulderBone = playerRenderData.shoulderBone;
		armDrawable->skeleton->setPosition(shoulderBone->getWorldX(), shoulderBone->getWorldY());
		armDrawable->animationState->update(dt);
		armDrawable->animationState->apply(*armDrawable->skeleton);
		if(isShooting)
		{
			playerRenderData.targetDir = neko::Vec2<float>(targetDir).Normalized();
		}
		float degree = std::acos(neko::Vec2<float>::Dot(playerRenderData.targetDir, { 0.0f, -1.0f })) / (float)M_PI * 180.0f;
		if(playerRenderData.state == PlayerRenderState::WALK_BACK)
		{
			//degree = -degree;
		}
		auto* rootBone = playerRenderData.armDrawable->skeleton->getRootBone();
		rootBone->setRotation(degree);
		rootBone->setAppliedRotation(degree);
		armDrawable->skeleton->update(dt);
		armDrawable->skeleton->updateWorldTransform(spine::Physics_Update);

		auto& gunDrawable = playerRenderData.gunDrawable;
		gunDrawable->skeleton->setScaleX((playerRenderData.faceRight ? 1.0f : -1.0f) * scale);
		gunDrawable->skeleton->setScaleY(scale);
		auto* handBone = playerRenderData.handBone;
		gunDrawable->skeleton->setPosition(handBone->getWorldX(), handBone->getWorldY());
		gunDrawable->animationState->update(dt);
		gunDrawable->animationState->apply(*gunDrawable->skeleton);

		rootBone = gunDrawable->skeleton->getRootBone();
		rootBone->setRotation(degree);
		rootBone->setAppliedRotation(degree);

		gunDrawable->skeleton->update(dt);
		gunDrawable->skeleton->updateWorldTransform(spine::Physics_Update);


		if(playerRenderData.isRespawning || !playerRenderData.cloudEndRespawnTimer.Over())
		{
			auto& cloudDrawable = playerRenderData.cloudDrawable;
			cloudDrawable->skeleton->setScaleX(scale);
			cloudDrawable->skeleton->setScaleY(scale);
			if (!playerCharacter.respawnPauseTimer.Over())
			{
				cloudDrawable->skeleton->setPosition((float)position.x, (float)position.y);
			}
			else if (!playerCharacter.respawnMoveTimer.Over())
			{
				const auto spawnPosition = neko::Vec2<float>(GetGraphicsPosition(PlayerManager::spawnPositions[playerNumber]));
				const auto physicsPosition = neko::Vec2<float>(position);
				const auto midPosition = physicsPosition + (spawnPosition - physicsPosition)
					* (float)playerCharacter.respawnMoveTimer.CurrentRatio();
				cloudDrawable->skeleton->setPosition(midPosition.x, midPosition.y);
			}
			else if (!playerCharacter.respawnStaticTime.Over())
			{
				const auto spawnPosition = neko::Vec2<float>(GetGraphicsPosition(PlayerManager::spawnPositions[playerNumber]));
				cloudDrawable->skeleton->setPosition(spawnPosition.x, spawnPosition.y);
			}

			playerRenderData.cloudEndRespawnTimer.Update(dt);
			cloudDrawable->update(dt, spine::Physics_Update);

		}
	}
}
}