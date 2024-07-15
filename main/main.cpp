#include "engine/engine.h"

#include "graphics/texture_manager.h"

#include <SDL.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <iostream>


#define DELAY 3000

int main(int argc, char** argv)
{
	splash::Engine engine;
	engine.Run();
	return 0;

/*
	bool isOpen = true;
	while (isOpen)
	{
		SDL_Event e;
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				isOpen = false;
			}
		}







	}






	return 0;
 */
}