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

static constexpr std::array<neko::Vec2f, 4> rockPositions
	{{
		 {neko::Scalar{-8.319638f}, neko::Scalar{-3.03f}},
		 {neko::Scalar{-0.5121031f}, neko::Scalar{-2.901045f}},
		 {neko::Scalar{1.822861f}, neko::Scalar{-3.582076f}},
		 {neko::Scalar{8.60885f}, neko::Scalar{-2.876722f}}
	}};

static constexpr std::array<std::pair<neko::Vec2f, TextureManager::TextureId>, 6> clouds
	{{
		 {{neko::Scalar{-11.14f}, neko::Scalar{2.79f}}, TextureManager::TextureId::CLOUD3},
		 {{neko::Scalar{-1.59f}, neko::Scalar{4.33f}}, TextureManager::TextureId::CLOUD1},
		 {{neko::Scalar{5.17f}, neko::Scalar{2.67f}}, TextureManager::TextureId::CLOUD1},
		 {{neko::Scalar{-4.549778f}, neko::Scalar{2.362802f}}, TextureManager::TextureId::CLOUD2},
		 {{neko::Scalar{-3.93f}, neko::Scalar{-0.03f}}, TextureManager::TextureId::CLOUD3},
		 {{neko::Scalar{5.071989f}, neko::Scalar{-0.4336729f}}, TextureManager::TextureId::CLOUD1}
	}};

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

	neko::Vec2f size = {static_cast<neko::Scalar>(static_cast<float>(gameWindowSize.x)/pixelPerMeter),
						static_cast<neko::Scalar>(static_cast<float>(gameWindowSize.y)/pixelPerMeter)};
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

	//Mid bg
	const neko::Vec2f midBgSize {static_cast<neko::Vec2<float>>(GetTextureSize(TextureManager::TextureId::MIDBG))/pixelPerMeter};
	const neko::Vec2f midBgPos{{}, neko::Scalar {-static_cast<float>(gameWindowSize.y)/pixelPerMeter/2.0f+static_cast<float>(midBgSize.y)/2.0f}};
	const auto midBgRect = GetDrawingRect(midBgPos, midBgSize);
	SDL_RenderCopy(renderer, midBgTexture_, nullptr, &midBgRect);

	for(std::size_t i = 0; i < rocks_.size(); i++)
	{
		const auto& rock = rocks_[i];
		neko::Vec2f rockPosition = rockPositions[i];
		rockPosition.y += neko::Scalar{rock.amplitude* std::sin(2.0f * neko::Pi<float>()*rock.floatingTime/rock.floatingPeriod)};
		neko::Vec2f rockSize{neko::Vec2<float>(rock.textureSize)/pixelPerMeter};
		const auto rockRect = GetDrawingRect(rockPosition, rockSize);
		SDL_RenderCopy(renderer, rock.texture, nullptr, &rockRect);
	}

	//Clouds
	for(std::size_t i = 0; i < clouds_.size(); i++)
	{
		const auto& cloud = clouds_[i];
		auto* texture = GetTexture(clouds[i].second);
		auto cloudPos = clouds[i].first;
		cloudPos.x = static_cast<neko::Scalar>(cloud.currentPosX);
		const neko::Vec2f cloudSize {static_cast<neko::Vec2<float>>(GetTextureSize(clouds[i].second))/pixelPerMeter};
		const auto cloudRect = GetDrawingRect(cloudPos, cloudSize);
		SDL_RenderCopyEx(renderer, texture, nullptr, &cloudRect, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
	}

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
	if(!IsTexturesLoaded())
	{
		return;
	}
	if(platformTextures_[0] == nullptr && IsTexturesLoaded())
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
	for(auto& rock : rocks_)
	{
		rock.floatingTime += dt;
		while(rock.floatingTime > rock.floatingPeriod)
		{
			rock.floatingTime -= rock.floatingPeriod;
		}
	}
	for(auto& cloud: clouds_)
	{
		cloud.currentPosX += cloud.speedX*dt;
		if(cloud.currentPosX > Cloud::maxDist)
		{
			cloud.currentPosX = -Cloud::maxDist;
		}
	}

}
void LevelView::LoadTextures()
{
	for(std::size_t i = 0; i < platformTextures_.size(); i++)
	{
		platformTextures_[i] = GetTexture(static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::PLAT1)+i));
	}
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> dis(0.0f, static_cast<float>(gameWindowSize.x));
	for(float & fogOffset : fogOffsets_)
	{
		fogOffset = dis(gen);
	}
	for(std::size_t i = 0; i < fogTextures_.size(); i++)
	{
		fogTextures_[i] = GetTexture(static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::BACKFOG)+i));
	}
	backgroundTexture_ = GetTexture(TextureManager::TextureId::BG);
	midBgTexture_ = GetTexture(TextureManager::TextureId::MIDBG);

	std::uniform_real_distribution<float> dis1(0.3f, 0.75f);
	std::uniform_real_distribution<float> dis2(5.0f, 15.0f);

	for(std::size_t i = 0; i < rocks_.size(); i++)
	{
		auto& rock = rocks_[i];
		const auto textureId = static_cast<TextureManager::TextureId>(static_cast<std::size_t>(TextureManager::TextureId::FLOATROCK1)+i);
		rock.texture = GetTexture(textureId);
		rock.textureSize = GetTextureSize(textureId);

		rock.amplitude = dis1(gen);
		rock.floatingPeriod = dis2(gen);
		std::uniform_real_distribution<float> dis3(0.0f, rock.floatingPeriod);
		rock.floatingTime = dis3(gen);
	}
	std::uniform_real_distribution<float> disCloud(0.1f, 0.4f);
	for(std::size_t i = 0; i < clouds_.size(); i++)
	{
		auto& cloud = clouds_[i];
		cloud.currentPosX = static_cast<float>(clouds[i].first.x);
		cloud.speedX = disCloud(gen);
	}
}
}