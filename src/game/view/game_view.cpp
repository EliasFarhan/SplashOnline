//
// Created by unite on 02.08.2024.
//
#include "game/graphics/game_view.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{
static GameView* instance = nullptr;
void GameView::Draw()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	levelRenderer_.Draw();
	playerRenderer_.Draw();
	bulletRenderer_.Draw();

	if(isGameOver_)
	{
		endScreenView_.Draw();
	}

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
void GameView::SetGraphicsIndex(int index)
{
	graphicIndex = index;
}
int GameView::GetGraphicsIndex() const
{
	return graphicIndex;
}
void GameView::Begin()
{
	AddDrawInterface(this);
	playerRenderer_.Begin();
	bulletRenderer_.Begin();
	levelRenderer_.Begin();
}
void GameView::Update(float dt)
{
	timeSinceTick_ += dt;
	levelRenderer_.Update(dt);
	playerRenderer_.Update(dt);
	bulletRenderer_.Update(dt);

	if(isGameOver_)
	{
		endScreenView_.Update(dt);
	}
}
void GameView::End()
{
	RemoveDrawInterface(this);
	levelRenderer_.End();
	bulletRenderer_.End();
	playerRenderer_.End();
}

GameView::GameView(const GameSystems* gameSystems):
	playerRenderer_(gameSystems),
	bulletRenderer_(gameSystems),
	gameSystems_(gameSystems)
{
	instance = this;
}

void GameView::Tick()
{
	timeSinceTick_ -= (float)fixedDeltaTime;
}

float GameView::GetTimeSinceTick() const
{
	return timeSinceTick_;
}
}