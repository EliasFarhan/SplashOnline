//
// Created by unite on 12.07.2024.
//

#include "graphics/graphics_manager.h"

namespace splash
{

void GraphicsManager::Begin()
{
	textureManager_.Begin();
}

void GraphicsManager::End()
{
	textureManager_.End();
}

void GraphicsManager::Update(float dt)
{
	if(!textureManager_.IsLoaded())
	{
		textureManager_.UpdateLoad();
	}
}

void GraphicsManager::Draw()
{
	SDL_Rect texture_rect;
	texture_rect.x = 0; //the x coordinate
	texture_rect.y = 0; //the y coordinate
	texture_rect.w = 800; //the width of the texture
	texture_rect.h = 600; //the height of the texture
	/*SDL_RenderCopy(renderer,
		textureManager.GetTexture(splash::TextureManager::TextureId::BG),
		NULL,
		&texture_rect);
	 */
}
}