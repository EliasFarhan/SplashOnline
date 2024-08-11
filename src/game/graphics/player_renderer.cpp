#include "game/graphics/player_renderer.h"

#include "graphics/graphics_manager.h"


namespace splash
{

void PlayerRenderer::Begin()
{

}
void PlayerRenderer::End()
{

}
void PlayerRenderer::Update([[maybe_unused]]float dt)
{
	if(IsSpineLoaded())
	{
		if(playerSkeletonDrawables_[0]== nullptr)
		{
			for(int i = 0; i < MaxPlayerNmb; i++)
			{
				playerSkeletonDrawables_[i] = CreateSkeletonDrawable((SpineManager::SkeletonId)((int)SpineManager::CAT_NOARM+i));
				playerSkeletonDrawables_[i]->animationState->setAnimation(0, "idle", true);
			}
		}
		else
		{
			for(int i = 0; i < MaxPlayerNmb; i++)
			{
				playerSkeletonDrawables_[i]->update(dt, spine::Physics_Update);
			}
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
			playerSkeletonDrawable->skeleton->setScaleX(scale);
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
}