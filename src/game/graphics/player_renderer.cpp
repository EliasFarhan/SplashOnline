#include "game/graphics/player_renderer.h"

#include "graphics/graphics_manager.h"

#include <math/fixed_lut.h>


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

void PlayerRenderer::Begin()
{

}
void PlayerRenderer::End()
{

}
void PlayerRenderer::Update([[maybe_unused]]float dt)
{
	if (!IsSpineLoaded())
		return;
	if(playerRenderDatas_[0].bodyDrawable == nullptr)
	{
		Load();
	}
	else
	{

		const auto& playerManager = gameSystems_->GetPlayerManager();
		for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
		{
			const auto& playerCharacter = playerManager.GetPlayerCharacter()[playerNumber];
			const auto& playerInput = playerManager.GetPlayerInputs()[playerNumber];
			auto& playerRenderData = playerRenderDatas_[playerNumber];

			const auto targetDir = neko::Vec2<neko::Fixed8>(playerInput.targetDirX, playerInput.targetDirY);
			if((playerInput.moveDirX > PlayerCharacter::deadZone && !playerRenderData.faceRight) ||
				(playerInput.moveDirX < -PlayerCharacter::deadZone && playerRenderData.faceRight))
			{
				playerRenderData.faceRight = !playerRenderData.faceRight;
			}
			float animRatio = 1.0f;
			switch(playerRenderDatas_[playerNumber].state)
			{

			case PlayerRenderState::IDLE:
			{
				if(playerCharacter.footCount <= 0)
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				if(neko::Abs<neko::Fixed8>(playerInput.moveDirX) > PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::WALK, playerNumber);
				}
				else
				{
					if(targetDir.SquareLength() > PlayerCharacter::deadZone)
					{
						SwitchToState(PlayerRenderState::SHOOT, playerNumber);
					}
				}
				break;
			}
			case PlayerRenderState::WALK:
			{
				if(playerCharacter.footCount <= 0)
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				if(neko::Abs(playerInput.moveDirX) < PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				else
				{
					if((playerRenderData.faceRight && playerInput.targetDirX < neko::Fixed8{0.0f}) ||
						(!playerRenderData.faceRight && playerInput.targetDirX > neko::Fixed8{0.0f}))
					{
						SwitchToState(PlayerRenderState::WALK_BACK, playerNumber);
					}
					animRatio = (float)neko::Abs(playerInput.moveDirX);
				}
				break;
			}
			case PlayerRenderState::WALK_BACK:
			{
				if(playerCharacter.footCount <= 0)
				{
					SwitchToState(PlayerRenderState::FALL, playerNumber);
				}
				if(neko::Abs<neko::Fixed8>(playerInput.moveDirX) < PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				else
				{
					if((!playerRenderData.faceRight && playerInput.targetDirX < neko::Fixed8{0.0f}) ||
					   (playerRenderData.faceRight && playerInput.targetDirX > neko::Fixed8{0.0f}))
					{
						SwitchToState(PlayerRenderState::WALK, playerNumber);
					}

					animRatio = (float)neko::Abs(playerInput.moveDirX);
				}
				break;
			}
			case PlayerRenderState::FALL:
			{
				if(playerCharacter.footCount > 0)
				{
					SwitchToState(PlayerRenderState::IDLE, playerNumber);
				}
				break;
			}
			case PlayerRenderState::SHOOT:
			{
				if(neko::Abs<neko::Fixed8>(playerInput.moveDirX) > PlayerCharacter::deadZone)
				{
					SwitchToState(PlayerRenderState::WALK, playerNumber);
				}
				else
				{
					if(targetDir.SquareLength() < PlayerCharacter::deadZone)
					{
						SwitchToState(PlayerRenderState::IDLE, playerNumber);
					}
					else
					{
						if((targetDir.x > PlayerCharacter::deadZone && !playerRenderData.faceRight) ||
							(targetDir.x < -PlayerCharacter::deadZone && playerRenderData.faceRight))
						{
							playerRenderData.faceRight = !playerRenderData.faceRight;
						}
					}
				}
				break;
			}
			case PlayerRenderState::JET:
				break;
			case PlayerRenderState::JETBURST:
				break;
			case PlayerRenderState::DASHPREP:
				break;
			case PlayerRenderState::DASH:
				break;
			case PlayerRenderState::BOUNCE:
				break;
			case PlayerRenderState::LENGTH:
				break;
			}
			playerRenderDatas_[playerNumber].bodyDrawable->update(animRatio*dt, spine::Physics_Update);
			playerRenderDatas_[playerNumber].armDrawable->update(dt, spine::Physics_Update);
			playerRenderDatas_[playerNumber].gunDrawable->update(dt, spine::Physics_Update);
		}
	}
}
void PlayerRenderer::Draw()
{
	auto* renderer = GetRenderer();
	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	const auto& playerManager = gameSystems_->GetPlayerManager();
	const auto& playerPhysics = playerManager.GetPlayerPhysics();
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		const auto& body = physicsWorld.body(playerPhysics[i].bodyIndex);
		if(playerRenderDatas_[i].bodyDrawable)
		{
			auto& bodyDrawable = playerRenderDatas_[i].bodyDrawable;
			const auto scale = playerScale * GetGraphicsScale();
			const auto position = GetGraphicsPosition(body.position);
			bodyDrawable->skeleton->setScaleX((playerRenderDatas_[i].faceRight ? 1.0f : -1.0f)*scale);
			bodyDrawable->skeleton->setScaleY(scale);
			bodyDrawable->skeleton->setPosition((float)position.x, (float)position.y);

			auto& armDrawable = playerRenderDatas_[i].armDrawable;
			armDrawable->skeleton->setScaleX((playerRenderDatas_[i].faceRight ? 1.0f : -1.0f)*scale);
			armDrawable->skeleton->setScaleY(scale);
			auto* shoulderBone = playerRenderDatas_[i].shoulderBone;
			armDrawable->skeleton->setPosition(shoulderBone->getWorldX(), shoulderBone->getWorldY());


			auto& gunDrawable = playerRenderDatas_[i].gunDrawable;
			gunDrawable->skeleton->setScaleX((playerRenderDatas_[i].faceRight ? 1.0f : -1.0f)*scale);
			gunDrawable->skeleton->setScaleY(scale);
			auto* handBone = playerRenderDatas_[i].handBone;
			gunDrawable->skeleton->setPosition(handBone->getWorldX(), handBone->getWorldY());

			// Draw in correct order
			bodyDrawable->draw(renderer);
			gunDrawable->draw(renderer);
			armDrawable->draw(renderer);
		}
		if(GetDebugConfig().showPhysicsBox)
		{
			const auto rect = GetDrawingRect(body.position + PlayerPhysic::box.offset, PlayerPhysic::box.size);
			const auto& color = playerColors[i];
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawRect(renderer, &rect);

			const auto
				footRect = GetDrawingRect(body.position + PlayerPhysic::footBox.offset, PlayerPhysic::footBox.size);
			SDL_RenderDrawRect(renderer, &footRect);
		}

	}
}

PlayerRenderer::PlayerRenderer(const GameSystems* gameSystems): gameSystems_(gameSystems)
{

}

void PlayerRenderer::Tick()
{

}

void PlayerRenderer::SwitchToState(PlayerRenderState state, int playerNumber)
{
	playerRenderDatas_[playerNumber].bodyDrawable->animationState->setAnimation(0,
		playerAnimNames[(int)state].data(), true);
	playerRenderDatas_[playerNumber].state = state;
}
void PlayerRenderer::Load()
{
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
	}
}
}