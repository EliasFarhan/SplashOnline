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
	if(playerSkeletonDrawables_[0] == nullptr)
	{
		for(int i = 0; i < MaxPlayerNmb; i++)
		{
			playerSkeletonDrawables_[i] = CreateSkeletonDrawable((SpineManager::SkeletonId)((int)SpineManager::CAT_NOARM+i));
			playerSkeletonDrawables_[i]->animationState->setAnimation(0, "idle", true);
		}
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
				if(playerCharacter.footCount <= 0)
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
			playerSkeletonDrawables_[playerNumber]->update(animRatio*dt, spine::Physics_Update);
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
		if(playerSkeletonDrawables_[i])
		{
			auto& playerSkeletonDrawable = playerSkeletonDrawables_[i];
			const auto scale = playerScale * GetGraphicsScale();
			const auto position = GetGraphicsPosition(body.position);
			playerSkeletonDrawable->skeleton->setScaleX((playerRenderDatas_[i].faceRight?1.0f:-1.0f)*scale);
			playerSkeletonDrawable->skeleton->setScaleY(scale);
			playerSkeletonDrawable->skeleton->setPosition((float)position.x, (float)position.y);
			playerSkeletonDrawable->draw(renderer);
		}
		const auto rect = GetDrawingRect(body.position+PlayerPhysic::box.offset, PlayerPhysic::box.size);
		const auto& color = playerColors[i];
		SDL_SetRenderDrawColor(renderer,color.r, color.g, color.b, color.a);
		SDL_RenderDrawRect(renderer,&rect);


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
	playerSkeletonDrawables_[playerNumber]->animationState->setAnimation(0,
		playerAnimNames[(int)state].data(), true);
	playerRenderDatas_[playerNumber].state = state;
}
}