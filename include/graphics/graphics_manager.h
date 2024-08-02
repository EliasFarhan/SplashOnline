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
	virtual void SetIndex(int index) = 0;
	[[nodiscard]] virtual int GetIndex() const = 0;
};

class GraphicsManager
{
public:
	GraphicsManager();
	void Begin();
	void Update(float dt);
	void End();
	[[nodiscard]] SDL_Renderer* GetRenderer() const;
	void PreDraw();
	void Draw();
	void PostDraw();
	[[nodiscard]] void AddDrawInterface(DrawInterface* drawInterface);
	void RemoveDrawInterface(DrawInterface* drawInterface);
private:
	TextureManager textureManager_;
	GuiRenderer guiRenderer_;
	SDL_Renderer* renderer_ = nullptr;
	std::vector<DrawInterface*> drawInterfaces_;
};

void AddDrawInterface(DrawInterface* drawInterface);
void RemoveDrawInterface(DrawInterface* drawInterface);
SDL_Renderer* GetRenderer();
}

#endif //SPLASHONLINE_GRAPHICS_MANAGER_H_
