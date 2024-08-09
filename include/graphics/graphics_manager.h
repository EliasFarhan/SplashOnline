//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_GRAPHICS_MANAGER_H_
#define SPLASHONLINE_GRAPHICS_MANAGER_H_

#include "graphics/texture_manager.h"
#include "graphics/gui_renderer.h"
#include "graphics/spine_manager.h"

#include <math/vec2.h>

#include <vector>

namespace splash
{

class DrawInterface
{
public:
	virtual void Draw() = 0;
	virtual void SetGraphicsIndex(int index) = 0;
	[[nodiscard]] virtual int GetGraphicsIndex() const = 0;
};

class GraphicsManager: public OnEventInterface
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


	SDL_Rect GetDrawingRect(neko::Vec2f position, neko::Vec2f size);

	void AddDrawInterface(DrawInterface* drawInterface);
	void RemoveDrawInterface(DrawInterface* drawInterface);

	void OnEvent(const SDL_Event& event) override;
	[[nodiscard]] int GetEventListenerIndex() const override;
	void SetEventListenerIndex(int index) override;
private:
	void ReloadDrawingSize();

	TextureManager textureManager_;
	SpineManager spineManager_;
	GuiRenderer guiRenderer_;
	SDL_Renderer* renderer_ = nullptr;
	std::vector<DrawInterface*> drawInterfaces_;
	neko::Vec2i windowSize_{};
	neko::Vec2i offset_{};
	neko::Vec2i actualSize_{};
	float scale_ = 1.0f;
	int eventListenerIndex = -1;
};

void AddDrawInterface(DrawInterface* drawInterface);
void RemoveDrawInterface(DrawInterface* drawInterface);
SDL_Renderer* GetRenderer();
SDL_Rect GetDrawingRect(neko::Vec2f position, neko::Vec2f size);
}

#endif //SPLASHONLINE_GRAPHICS_MANAGER_H_
