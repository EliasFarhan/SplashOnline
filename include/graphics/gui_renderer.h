//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_GUI_RENDERER_H_
#define SPLASHONLINE_GUI_RENDERER_H_

#include "engine/window.h"

#include <SDL.h>
#include <vector>

namespace splash
{

class OnGuiInterface
{
public:
	virtual void OnGui() = 0;
	virtual void SetGuiIndex(int index) = 0;
	[[nodiscard]] virtual int GetGuiIndex() const = 0;
};

class GuiRenderer : public OnEventInterface
{
public:
	GuiRenderer();
	void Begin();
	void Update();
	void End();

	void Draw();

	void AddGuiInterface(OnGuiInterface* guiInterface);
	void RemoveGuiInterface(OnGuiInterface* guiInterface);
	void OnEvent(const SDL_Event& event) override;
private:
	SDL_Renderer* renderer_;
	std::vector<OnGuiInterface*> guiInterfaces_;
};

void AddGuiInterface(OnGuiInterface* guiInterface);
void RemoveGuiInterface(OnGuiInterface* guiInterface);
}

#endif //SPLASHONLINE_GUI_RENDERER_H_
