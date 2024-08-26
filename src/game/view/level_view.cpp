//
// Created by unite on 08.08.2024.
//
#include "game/graphics/level_view.h"
#include "graphics/texture_manager.h"
#include "graphics/const.h"
#include "graphics/graphics_manager.h"
#include "game/level.h"

#include <math/vec2.h>

#include <random>

namespace splash
{

void LevelView::Begin()
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> dis(0.0f, (float)gameWindowSize.x);
	for(float & fogOffset : fogOffsets_)
	{
		fogOffset = dis(gen);
	}
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

	//Back fog
	auto* bgFogTexture = GetTexture(TextureManager::TextureId::BACKFOG);
	const neko::Vec2f bgFogSize{neko::Vec2<float>(GetTextureSize(TextureManager::TextureId::BACKFOG))/pixelPerMeter};
	auto bgFogRect = GetDrawingRect(neko::Vec2f{neko::Scalar {fogOffsets_[0]},
												neko::Scalar{-gameWindowSize.y/2.0f/pixelPerMeter}+bgFogSize.y/neko::Scalar{2.0f} }, bgFogSize);
	SDL_RenderCopy(renderer, bgFogTexture, nullptr, &bgFogRect);
	bgFogRect.x -= GetActualGameSize().x-1;
	SDL_RenderCopy(renderer, bgFogTexture, nullptr, &bgFogRect);

	//TODO draw bg behind the front fog

	//Front fog
	auto* frontFogTexture = GetTexture(TextureManager::TextureId::FRONTFOG);
	const neko::Vec2f frontFogSize{neko::Vec2<float>(GetTextureSize(TextureManager::TextureId::FRONTFOG))/pixelPerMeter};
	auto frontFogRect = GetDrawingRect(neko::Vec2f{neko::Scalar {fogOffsets_[1]},
												neko::Scalar{-gameWindowSize.y/2.0f/pixelPerMeter}+frontFogSize.y/neko::Scalar{2.0f} }, frontFogSize);
	SDL_RenderCopy(renderer, frontFogTexture, nullptr, &frontFogRect);
	frontFogRect.x -= GetActualGameSize().x-1;
	SDL_RenderCopy(renderer, frontFogTexture, nullptr, &frontFogRect);

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
void LevelView::Update(float dt)
{
	if(platformTextures_[0] == nullptr && IsTextureLoaded())
	{
		LoadTextures();
	}
	fogOffsets_[0] += 0.5f*dt;
	if(fogOffsets_[0] > gameWindowSize.x/pixelPerMeter)
	{
		fogOffsets_[0] -= gameWindowSize.x/pixelPerMeter;
	}

	fogOffsets_[1] += dt;
	if(fogOffsets_[1] > gameWindowSize.x/pixelPerMeter)
	{
		fogOffsets_[1] -= gameWindowSize.x/pixelPerMeter;
	}
}
void LevelView::LoadTextures()
{
	for(std::size_t i = 0; i < platformTextures_.size(); i++)
	{
		platformTextures_[i] = GetTexture((TextureManager::TextureId)((int)TextureManager::TextureId::PLAT1+i));
	}
	for(std::size_t i = 0; i < fogTextures_.size(); i++)
	{
		fogTextures_[i] = GetTexture((TextureManager::TextureId)((int)TextureManager::TextureId::BACKFOG+i));
	}
	backgroundTexture_ = GetTexture(TextureManager::TextureId::BG);
}
}