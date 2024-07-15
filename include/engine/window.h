//
// Created by unite on 14.07.2024.
//

#ifndef SPLASHONLINE_WINDOW_H_
#define SPLASHONLINE_WINDOW_H_

#include <SDL.h>

namespace splash
{
class OnEventInterface
{
public:
	virtual void OnEvent(const SDL_Event& event) = 0;
};

class Window
{
public:
	Window();
	void Begin();
	void End();
	SDL_Window* GetWindow();
private:
	SDL_Window* window_;
};

SDL_Window* GetWindow();
}
#endif //SPLASHONLINE_WINDOW_H_
