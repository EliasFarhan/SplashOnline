//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE__GUI_RENDERER_H_
#define SPLASHONLINE__GUI_RENDERER_H_

#include <SDL.h>
#include <vector>

namespace splash
{

class OnGuiInterface
{
public:
	virtual void OnGui() = 0;
};

class GuiRenderer
{
public:
	void Begin();

	void End();

	void Draw();

	void AddListener(OnGuiInterface* guiInterface);
private:
	SDL_Renderer* renderer_;
	std::vector<OnGuiInterface*> guiInterfaces_;
};
}


#endif //SPLASHONLINE__GUI_RENDERER_H_
