//
// Created by unite on 08.08.2024.
//
#include "game/graphics/level_view.h"
#include "graphics/texture_manager.h"
#include "graphics/const.h"
#include "graphics/graphics_manager.h"
#include "game/level.h"

#include <math/vec2.h>

namespace splash
{

void LevelView::Begin()
{

}
void LevelView::End()
{

}

void LevelView::Draw()
{
	if(backgroundTexture_ == nullptr)
	{
		return;
	}
	auto* renderer = GetRenderer();

	neko::Vec2f size = {(neko::Scalar )((float)gameWindowSize.x/pixelPerMeter),(neko::Scalar )((float)gameWindowSize.y/pixelPerMeter)};
	const auto bgRect = GetDrawingRect({}, size);
	SDL_RenderCopy(renderer,
		backgroundTexture_,
		nullptr,
		&bgRect);

	const auto& platforms = Level::platforms;
	for(std::size_t i = 0 ; i < platformTextures_.size(); i++)
	{
		const auto platformRect = GetDrawingRect(platforms[i].position+platforms[i].offset, platforms[i].size);
		SDL_RenderCopy(renderer,
			platformTextures_[i],
			nullptr,
			&platformRect);
		if(GetDebugConfig().showPhysicsBox)
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
			SDL_RenderDrawRect(renderer, &platformRect);
		}
	}
	const auto& gameLimits = Level::gameLimits;
	for(const auto& gameLimit : gameLimits)
	{
		if(GetDebugConfig().showPhysicsBox)
		{
			const auto gameLimitRect = GetDrawingRect(
				gameLimit.first.position+gameLimit.first.offset,
				gameLimit.first.size);

			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
			SDL_RenderFillRect(renderer, &gameLimitRect);
		}
	}
}
void LevelView::Update()
{
	if(platformTextures_[0] == nullptr && IsTextureLoaded())
	{
		LoadTextures();
	}
}
void LevelView::LoadTextures()
{
	for(std::size_t i = 0; i < platformTextures_.size(); i++)
	{
		platformTextures_[i] = GetTexture((TextureManager::TextureId)((int)TextureManager::TextureId::PLAT1+i));
	}
	backgroundTexture_ = GetTexture(TextureManager::TextureId::BG);
}
}