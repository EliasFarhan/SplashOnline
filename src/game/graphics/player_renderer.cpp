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