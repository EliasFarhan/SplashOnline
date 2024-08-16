//
// Created by unite on 02.08.2024.
//
#include "game/graphics/game_renderer.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{
static GameRenderer* instance = nullptr;
void GameRenderer::Draw()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	levelRenderer_.Draw();
	playerRenderer_.Draw();
	bulletRenderer_.Draw();
	if(GetDebugConfig().showPhysicsBox)
	{
		auto* renderer = GetRenderer();
		std::function<void(const neko::QuadNode*)> drawQuad = [renderer, &drawQuad](const neko::QuadNode* node)
		{
			const auto& aabb = node->aabb;
			const auto center = aabb.GetCenter();
			const auto halfSize = aabb.GetHalfSize();
			const auto quadRect = GetDrawingRect(center, halfSize+halfSize);
			SDL_RenderDrawRect(renderer, &quadRect);
			if(node->nodes[0] != nullptr)
			{
				for(const auto childNode : node->nodes)
				{
					drawQuad(childNode);
				}
			}
		};
		SDL_SetRenderDrawColor(renderer, 255,0,0,255);
		drawQuad(&gameSystems_->GetQuadTree().GetRootNode());
	}
}
void GameRenderer::SetGraphicsIndex(int index)
{
	graphicIndex = index;
}
int GameRenderer::GetGraphicsIndex() const
{
	return graphicIndex;
}
void GameRenderer::Begin()
{
	AddDrawInterface(this);
	playerRenderer_.Begin();
	bulletRenderer_.Begin();
	levelRenderer_.Begin();
}
void GameRenderer::Update(float dt)
{
	timeSinceTick_ += dt;
	levelRenderer_.Update();
	playerRenderer_.Update(dt);
	bulletRenderer_.Update(dt);
}
void GameRenderer::End()
{
	RemoveDrawInterface(this);
	levelRenderer_.End();
	bulletRenderer_.End();
	playerRenderer_.End();
}

GameRenderer::GameRenderer(const GameSystems* gameSystems):
	playerRenderer_(gameSystems),
	bulletRenderer_(gameSystems),
	gameSystems_(gameSystems)
{
	instance = this;
}

void GameRenderer::Tick()
{
	timeSinceTick_ -= (float)fixedDeltaTime;
}

float GameRenderer::GetTimeSinceTick() const
{
	return timeSinceTick_;
}
}