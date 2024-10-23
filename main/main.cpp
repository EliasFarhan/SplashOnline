#include "engine/engine.h"
#include "game/splash.h"
#include "utils/log.h"

#include <SDL_main.h>
#include <argh.h>
#include <fmt/format.h>



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	auto cmdl = argh::parser(argc, argv);

	std::string input_file;
	if ((cmdl({ "-i", "--input"}) >> input_file))
	{
		splash::LogDebug(fmt::format("Using player inputs from {}", input_file));
	}

	splash::Engine engine;
	splash::AudioManager audioManager;
	splash::SplashManager splashManager;
	engine.Run();
	return 0;
}