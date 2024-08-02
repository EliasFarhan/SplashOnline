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
};

class GuiRenderer : public OnEventInterface
{
public:
	void Begin();
	void Update();
	void End();

	void Draw();

	void AddListener(OnGuiInterface* guiInterface);
	void OnEvent(const SDL_Event& event) override;
private:
	SDL_Renderer* renderer_;
	std::vector<OnGuiInterface*> guiInterfaces_;
};
}


#endif //SPLASHONLINE__GUI_RENDERER_H_
