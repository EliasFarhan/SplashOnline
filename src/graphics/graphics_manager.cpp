//
// Created by unite on 12.07.2024.
//

#include "graphics/graphics_manager.h"
#include "engine/window.h"
#include "utils/log.h"

#include <fmt/format.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

void GraphicsManager::Begin()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	auto* window = GetWindow();
	renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if(renderer_ == nullptr)
	{
		LogError(fmt::format("SDL renderer failed to initialise: {}\n", SDL_GetError()));

		return;
	}
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer_, &info);

	LogDebug(fmt::format("Renderer: {}", info.name));

	textureManager_.Begin();
	guiRenderer_.Begin();
}

void GraphicsManager::End()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	guiRenderer_.End();
	textureManager_.End();
	SDL_DestroyRenderer(renderer_);
}

static GraphicsManager* instance = nullptr;

GraphicsManager::GraphicsManager()
{
	instance = this;
}

SDL_Renderer* GraphicsManager::GetRenderer() const
{
	return renderer_;
}

SDL_Renderer* GetRenderer()
{
	return instance->GetRenderer();
}

void AddDrawInterface(DrawInterface* drawInterface)
{
	instance->AddDrawInterface(drawInterface);
}

void RemoveDrawInterface(DrawInterface* drawInterface)
{
	instance->RemoveDrawInterface(drawInterface);
}

void GraphicsManager::Update([[maybe_unused]]float dt)
{
	if(!textureManager_.IsLoaded())
	{
		textureManager_.UpdateLoad();
	}
	guiRenderer_.Update();
}

void GraphicsManager::Draw()
{
	if(textureManager_.IsLoaded())
	{
		auto [width, height] = GetWindowSize();
		SDL_Rect texture_rect;
		texture_rect.x = 0; //the x coordinate
		texture_rect.y = 0; //the y coordinate
		texture_rect.w = width; //the width of the texture
		texture_rect.h = height; //the height of the texture
		SDL_RenderCopy(renderer_,
				textureManager_.GetTexture(splash::TextureManager::TextureId::BG),
				nullptr,
				&texture_rect);
	}
	for(auto* drawInterface: drawInterfaces_)
	{
		drawInterface->Draw();
	}
	guiRenderer_.Draw();
}
void GraphicsManager::PreDraw()
{
	//Clear screen
	SDL_RenderClear(renderer_);
}
void GraphicsManager::PostDraw()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	// Rendering
	SDL_RenderPresent(renderer_);
}
void GraphicsManager::AddDrawInterface(DrawInterface* drawInterface)
{
	int index = -1;
	auto it = std::find(drawInterfaces_.begin(), drawInterfaces_.end(), nullptr);
	if(it == drawInterfaces_.end())
	{
		index = (int)drawInterfaces_.size();
		drawInterfaces_.push_back(drawInterface);
	}
	else
	{
		*it = drawInterface;
		index = (int)std::distance(drawInterfaces_.begin(), it);
	}

	drawInterface->SetIndex(index);
}
void GraphicsManager::RemoveDrawInterface(DrawInterface* drawInterface)
{
	drawInterfaces_[drawInterface->GetIndex()] = nullptr;
}
}