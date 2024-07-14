//
// Created by unite on 12.07.2024.
//

#include "graphics/graphics_manager.h"
#include "engine/window.h"

#include <iostream>

namespace splash
{

void GraphicsManager::Begin()
{
	auto* window = GetWindow();
	renderer_ = SDL_CreateRenderer(window, -1, 0);
	if(renderer_ == nullptr)
	{
		fprintf(stderr, "SDL renderer failed to initialise: %s\n", SDL_GetError());

		return;
	}
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer_, &info);

	std::cout << "SDL Renderer name: " << info.name << '\n';

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
}