//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_GRAPHICS_MANAGER_H_
#define SPLASHONLINE_GRAPHICS_MANAGER_H_

#include "graphics/texture_manager.h"
#include "graphics/gui_renderer.h"

namespace splash
{

class DrawInterface
{
public:
	virtual void Draw() = 0;
};

class GraphicsManager
{
public:
	GraphicsManager();
	void Begin();
	void Update(float dt);
	void End();
	[[nodiscard]] SDL_Renderer* GetRenderer() const;
	void Draw();
private:
	TextureManager textureManager_;
	GuiRenderer guiRenderer_;
	SDL_Renderer* renderer_;
};


SDL_Renderer* GetRenderer();
}

#endif //SPLASHONLINE_GRAPHICS_MANAGER_H_
