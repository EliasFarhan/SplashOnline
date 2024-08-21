#include "engine/engine.h"
#include "game/splash.h"

#include <SDL_main.h>



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::Engine engine;
	splash::AudioManager audioManager;
	splash::SplashManager splashManager;
	engine.Run();
	return 0;
}