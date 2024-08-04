
#include "engine/engine.h"
#include "network/client.h"

#include <SDL_main.h>



int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{

	splash::Engine engine;
	splash::NetworkClient client;

	engine.Run();
    return 0;
}