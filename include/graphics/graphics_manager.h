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
	virtual ~DrawInterface() = default;
	virtual void Draw() = 0;
	virtual void SetGraphicsIndex(int index) = 0;
	[[nodiscard]] virtual int GetGraphicsIndex() const = 0;
};


void BeginGraphics();
void UpdateGraphics();
void DrawGraphics();
void EndGraphics();
void ManageGraphicsEvent(const SDL_Event& event);

void AddDrawInterface(DrawInterface* drawInterface);
void RemoveDrawInterface(DrawInterface* drawInterface);
SDL_Renderer* GetRenderer();
SDL_Rect GetDrawingRect(neko::Vec2f position, neko::Vec2f size);
float GetGraphicsScale();
neko::Vec2i GetGraphicsPosition(neko::Vec2f position);
neko::Vec2i GetActualGameSize();
neko::Vec2i GetOffset();
}

#endif //SPLASHONLINE_GRAPHICS_MANAGER_H_
