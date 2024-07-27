//
// Created by unite on 12.07.2024.
//

#include "graphics/graphics_manager.h"
#include "engine/window.h"
#include "utils/log.h"

#include <fmt/format.h>


namespace splash
{

void GraphicsManager::Begin()
{
	auto* window = GetWindow();
	renderer_ = SDL_CreateRenderer(window, -1, 0);
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
	guiRenderer_.Draw();
}
void GraphicsManager::PreDraw()
{
	//Clear screen
	SDL_RenderClear(renderer_);
}
void GraphicsManager::PostDraw()
{
	// Rendering
	SDL_RenderPresent(renderer_);
}
void GraphicsManager::AddDrawInterface(DrawInterface* drawInterface)
{
	drawInterfaces_.push_back(drawInterface);
}
}