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
}