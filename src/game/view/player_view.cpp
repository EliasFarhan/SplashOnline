#include "game/graphics/player_view.h"
#include "graphics/graphics_manager.h"
#include "utils/log.h"
#include "rollback/rollback_manager.h"
#include "audio/audio_manager.h"
#include "audio/player_sound.h"
#include "graphics/const.h"

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
	for(auto& playerSound : playerSoundDatas_)
	{
		playerSound.jetpackSoundInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		playerSound.jetpackSoundInstance = nullptr;
	}
}
void PlayerView::Update([[maybe_unused]]float dt)
{
	if (!IsSpineLoaded())
		return;
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		if (IsValid(i) && playerRenderDatas_[i].bodyDrawable == nullptr)
		{
			Load();
			return;
		}
	}
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	const auto& playerManager = gameSystems_->GetPlayerManager();

	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		if(!IsValid(playerNumber))
		{
			continue;
		}
		const auto& playerCharacter = playerManager.GetPlayerCharacter()[playerNumber];
		const auto& playerInput = playerManager.GetPlayerInputs()[playerNumber];
		const auto& body = gameSystems_->GetPhysicsWorld().body(playerManager.GetPlayerPhysics()[playerNumber].bodyIndex);
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
			if(playerCharacter.IsRespawning())
			{
				auto ejectPosition = body.position;
				auto angle = 0.0f;
				neko::Vec2f bounds{(neko::Vec2<float>)gameWindowSize/pixelPerMeter/2.0f};

				if(ejectPosition.x < -bounds.x)
				{
					ejectPosition.x = -bounds.x;
					angle = -90.0f;
				}

				if(ejectPosition.x > bounds.x)
				{
					ejectPosition.x = bounds.x;
					angle = 90.0f;
				}

				if(ejectPosition.y < -bounds.y)
				{
					ejectPosition.y = -bounds.y;
					if(angle == 0.0f)
					{
						angle = 0.0f;
					}
					else if(angle == 90.0f)
					{
						angle = 45.0f;
					}
					else if(angle == -90.0f)
					{
						angle = -45.0f;
					}
				}
				if(ejectPosition.y > bounds.y)
				{
					ejectPosition.y = bounds.y;
					if(angle == 0.0f)
					{
						angle = 180.0f;
					}
					else if(angle == 90.0f)
					{
						angle = 45.0f+90.0f;
					}
					else if(angle == -90.0f)
					{
						angle = -45.0f-90.0f;
					}
				}

				playerRenderData.ejectFx.StartAnim("eject", ejectPosition, GetGraphicsScale(), angle);
				FmodPlaySound(GetPlayerDeathSoundEvent((Character)((int)Character::CAT+playerNumber)));
				FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::EJECT));
				playerRenderData.isRespawning = true;
				playerRenderData.cloudDrawable->animationState->setAnimation(0, "respawn", true);
				SwitchToState(PlayerRenderState::IDLE, playerNumber);
				playerRenderData.cloudDrawable->update(dt, spine::Physics_Update);
				continue;
			}

			if(playerCharacter.IsDashPrepping() && playerRenderData.state != PlayerRenderState::DASHPREP)
			{
				FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::STOMPPREP));
				SwitchToState(PlayerRenderState::DASHPREP, playerNumber);
				playerRenderData.dashPrepFx.StartAnim("dashprep", body.position,{0.5f * GetGraphicsScale()});
			}
			if(playerCharacter.IsDashing() && playerRenderData.state != PlayerRenderState::DASH)
			{
				FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::STOMPING));
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

							playerSoundDatas_[playerNumber].jetpackSoundInstance->setParameterValue("Transition Jetpack", 0.0f);
							if(playerCharacter.preJetBurstTimer.Over())
							{
								FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::JETPACKON));
							}
						}
						else
						{
							const auto animName = fmt::format("jetfuse_{}", legacyPlayerColorNames[playerNumber]);
							playerRenderData.jetBurstFx.StartAnim(
								animName,
								body.position,
								neko::Vec2<float>(playerScale * GetGraphicsScale()));
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
					const auto animName = fmt::format("jetfuse_{}", legacyPlayerColorNames[playerNumber]);
					playerRenderData.jetBurstFx.StartAnim(
						animName,
						body.position,
						neko::Vec2<float>(playerScale * GetGraphicsScale()));
					SwitchToState(PlayerRenderState::JETBURST, playerNumber);
				}
				if(playerRenderData.state != PlayerRenderState::JET)
				{
					playerSoundDatas_[playerNumber].jetpackSoundInstance->setParameterValue("Transition Jetpack", 0.75f);
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
						playerSoundDatas_[playerNumber].jetpackSoundInstance->setParameterValue("Transition Jetpack", 0.0f);

						FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::JETPACKON));
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

		if(!IsValid(i))
		{
			continue;
		}
		if (!playerRenderDatas_[i].bodyDrawable)
		{
			continue;
		}
		const auto& body = physicsWorld.body(playerPhysics[i].bodyIndex);
		auto& bodyDrawable = playerRenderDatas_[i].bodyDrawable;
		auto& armDrawable = playerRenderDatas_[i].armDrawable;
		auto& gunDrawable = playerRenderDatas_[i].gunDrawable;

		playerRenderDatas_[i].jetBurstFx.Draw();
		playerRenderDatas_[i].dashPrepFx.Draw();
		playerRenderDatas_[i].ejectFx.Draw();
		// Draw in correct order
		bodyDrawable->draw(renderer);
		if(playerRenderDatas_[i].state != PlayerRenderState::DASHPREP && playerRenderDatas_[i].state != PlayerRenderState::DASH)
		{
			gunDrawable->draw(renderer);
			armDrawable->draw(renderer);
		}
		if (playerRenderDatas_[i].isRespawning || !playerRenderDatas_[i].cloudEndRespawnTimer.Over())
		{
			auto& cloudDrawable = playerRenderDatas_[i].cloudDrawable;
			cloudDrawable->draw(renderer);
		}

		const neko::Aabbf playerAabb = neko::Aabbf::FromCenter(body.position+PlayerPhysic::box.offset, PlayerPhysic::box.size/neko::Scalar {2});
		const neko::Vec2f windowHalfSize = neko::Vec2f {neko::Vec2<float>{gameWindowSize}/pixelPerMeter/2.0f};
		const neko::Aabbf screenAabb = neko::Aabbf::FromCenter({}, windowHalfSize);
		if(!Intersect(screenAabb,playerAabb) && !playerManager.GetPlayerCharacter()[i].IsRespawning())
		{
			//Player is outside, draw oob
			static constexpr neko::Vec2f bgSize{neko::Scalar{0.25f*3.15f}};
			neko::Vec2f position = body.position;
			if(body.position.x < -windowHalfSize.x)
			{
				position.x = -windowHalfSize.x+neko::Scalar {1.0f};
			}
			if(body.position.x > windowHalfSize.x)
			{
				position.x = windowHalfSize.x-neko::Scalar {1.0f};
			}
			if(body.position.y < -windowHalfSize.y)
			{
				position.y = -windowHalfSize.y+neko::Scalar {1};
			}
			if(body.position.y > windowHalfSize.y)
			{
				position.y = windowHalfSize.y-neko::Scalar {1};
			}
			auto rect = GetDrawingRect(position, bgSize);
			SDL_RenderCopy(renderer, playerRenderDatas_[i].outIconBg, nullptr, &rect);

			rect = GetDrawingRect(position, (neko::Vec2f )
				(neko::Vec2<float>(GetTextureSize((TextureManager::TextureId)((int)TextureManager::TextureId::HEAD_P1_CAT+i)))/pixelPerMeter*0.5f));
			SDL_RenderCopy(renderer, playerRenderDatas_[i].outIconCharFace, nullptr, &rect);

			static constexpr neko::Vec2f arrowSize(neko::Scalar {34.0f/pixelPerMeter}, neko::Scalar{38.0f/pixelPerMeter});

			const auto delta = neko::Vec2<float>(body.position - position).Normalized();
			const auto angle = std::atan2(-delta.y,neko::Vec2<float>::Dot(delta, neko::Vec2<float>::right() ))/M_PI*180.0f;
			const auto arrowPosition = position+neko::Vec2f{delta} *neko::Scalar{0.62f};
			rect = GetDrawingRect(arrowPosition, arrowSize);
			SDL_RenderCopyEx(renderer, playerRenderDatas_[i].outIconArrow, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE );
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
			const auto right =
				GetDrawingRect(body.position + PlayerPhysic::rightBox.position+PlayerPhysic::rightBox.offset,
					PlayerPhysic::rightBox.size);
			SDL_RenderDrawRect(renderer, &right);
			const auto leftRect =
				GetDrawingRect(body.position + PlayerPhysic::leftBox.position+PlayerPhysic::leftBox.offset,
					PlayerPhysic::leftBox.size);
			SDL_RenderDrawRect(renderer, &leftRect);

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
		if(!IsValid(i))
		{
			continue;
		}
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
		playerRenderDatas_[i].cloudDrawable->animationState->setAnimation(0, "respawn", true);

		playerRenderDatas_[i].outIconBg = GetTexture((TextureManager::TextureId)((int)TextureManager::TextureId::OOB_P1_CYAN+i));
		playerRenderDatas_[i].outIconCharFace = GetTexture((TextureManager::TextureId)((int)TextureManager::TextureId::HEAD_P1_CAT+i));
		playerRenderDatas_[i].outIconArrow = GetTexture(TextureManager::TextureId::RIGHT_ARROW);

		playerSoundDatas_[i].jetpackSoundInstance = FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::JETPACK));
		playerSoundDatas_[i].jetpackSoundInstance->setParameterValue("Transition Jetpack", 0.75f);


		const auto animName = fmt::format("jetfuse_{}", legacyPlayerColorNames[i]);
		playerRenderDatas_[i].jetBurstFx.Create(SpineManager::JETBOOM, animName);
		playerRenderDatas_[i].dashPrepFx.Create(SpineManager::DASH_PREP, "dashprep");
		playerRenderDatas_[i].ejectFx.Create(SpineManager::EJECT, "eject");
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

		if(!IsValid(playerNumber))
		{
			continue;
		}
		auto& playerRenderData = playerRenderDatas_[playerNumber];
		const auto& playerCharacter = playerManager.GetPlayerCharacter()[playerNumber];
		const auto& body = physicsWorld.body(playerPhysics[playerNumber].bodyIndex);
		const auto position = GetGraphicsPosition(body.position);
		if (!playerRenderData.bodyDrawable)
		{
			continue;
		}

		playerRenderData.jetBurstFx.Update(dt);
		playerRenderData.dashPrepFx.Update(dt);
		playerRenderData.ejectFx.Update(dt);

		auto& bodyDrawable = playerRenderData.bodyDrawable;
		bodyDrawable->skeleton->setScaleX((playerRenderData.faceRight ? 1.0f : -1.0f) * scale);
		bodyDrawable->skeleton->setScaleY(scale);
		if (!playerRenderData.isRespawning)
		{

			if(playerCharacter.IsJetBursting())
			{
				float deltaX = 0.2f * playerScale * std::sin(2.0f*(float)neko::pi/0.25f*(float)playerCharacter.preJetBurstTimer.CurrentRatio());
				const auto jetburstPosition = GetGraphicsPosition(body.position+neko::Vec2f{neko::Scalar{deltaX}, {}});
				bodyDrawable->skeleton->setPosition((float)jetburstPosition.x, (float)jetburstPosition.y);
			}
			else
			{
				bodyDrawable->skeleton->setPosition((float)position.x, (float)position.y);
			}
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

		bodyDrawable->animationState->update(animRatio * dt);
		bodyDrawable->animationState->apply(*bodyDrawable->skeleton);
		float moveAngle = 0.0f;
		switch(playerRenderData.state)
		{
		case PlayerRenderState::IDLE:
		case PlayerRenderState::SHOOT:
		case PlayerRenderState::DASHPREP:
		case PlayerRenderState::WALK:
		case PlayerRenderState::WALK_BACK:
		{
			moveAngle = 0.0f;
			break;
		}
		case PlayerRenderState::JET:
		case PlayerRenderState::JETBURST:
		case PlayerRenderState::FALL:
		case PlayerRenderState::DASH:
		case PlayerRenderState::BOUNCE:
		{
			moveAngle = -(float)playerInputs[playerNumber].moveDirX*30.0f*(playerRenderData.faceRight?1.0f:-1.0f);
			break;
		}
		default:
			break;
		}
		if(playerCharacter.hitTimer.CurrentRatio() < neko::Scalar{0.25f} && playerRenderData.state != PlayerRenderState::DASHPREP)
		{
			switch(playerCharacter.resistancePhase)
			{
			case 3:
			{
				moveAngle = 7.5f*std::cos(2.0f*(float)neko::Pi<neko::Scalar>()/0.1f*(float)playerCharacter.hitTimer.RemainingTime());
				break;
			}
			case 2:
			{
				moveAngle += 3.5f*std::cos(2.0f*(float)neko::Pi<neko::Scalar>()/0.1f*(float)playerCharacter.hitTimer.RemainingTime());
				break;
			}
			case 1:
			{
				moveAngle += std::cos(2.0f*(float)neko::Pi<neko::Scalar>()/0.1f*(float)playerCharacter.hitTimer.RemainingTime());
				break;
			}
			default:
				break;
			}
		}
		auto* rootBone = playerRenderData.bodyDrawable->skeleton->getRootBone();
		rootBone->setRotation(moveAngle);
		rootBone->setAppliedRotation(moveAngle);
		bodyDrawable->skeleton->update(animRatio * dt);
		bodyDrawable->skeleton->updateWorldTransform(spine::Physics_Update);

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

		rootBone = playerRenderData.armDrawable->skeleton->getRootBone();
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
void PlayerRenderData::VisualFx::Create(SpineManager::SkeletonId skeletonId, std::string_view animName)
{
	drawable = CreateSkeletonDrawable(skeletonId);
	animationTimer.SetPeriod(drawable->skeleton->getData()->findAnimation(animName.data())->getDuration());
}
void PlayerRenderData::VisualFx::StartAnim(std::string_view animName, neko::Vec2f position, neko::Vec2<float> scale, float angle)
{
	drawable->skeleton->setToSetupPose();
	drawable->animationState->setAnimation(0, animName.data(), false);
	const auto screenPos = GetGraphicsPosition(position);
	drawable->skeleton->setPosition((float)screenPos.x, (float)screenPos.y);
	drawable->skeleton->setScaleX(scale.x);
	drawable->skeleton->setScaleY(scale.y);
	angle_ = angle;
	animationTimer.Reset();
}
void PlayerRenderData::VisualFx::Update(float dt)
{
	if(!animationTimer.Over())
	{
		animationTimer.Update(dt);
		drawable->animationState->update(dt);
		drawable->animationState->apply(*drawable->skeleton);
		if(angle_ != 0.0f)
		{
			auto* root = drawable->skeleton->getRootBone();
			root->setRotation(angle_);
			root->setAppliedRotation(angle_);
		}
		drawable->skeleton->update(dt);
		drawable->skeleton->updateWorldTransform(spine::Physics_Update);
	}
}
void PlayerRenderData::VisualFx::Draw()
{
	if(!animationTimer.Over())
	{
		auto* renderer = GetRenderer();
		drawable->draw(renderer);
	}
}
}