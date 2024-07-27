#include "engine/engine.h"

#include <SDL_main.h>



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::Engine engine;
	engine.Run();
	return 0;
}