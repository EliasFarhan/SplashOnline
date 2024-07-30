
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <SDL.h>
#include <SDL_main.h>

#include <array>
#include <vector>
#include <string_view>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	FMOD::Studio::System* system;
	if(FMOD::Studio::System::create(&system) != FMOD_OK)
	{
		return 1;
	}
	if(system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr))
	{
		return 1;
	}
	constexpr std::array<std::string_view, 3> bankNames =
	{{
		"data/sounds/Master Bank.bank",
		"data/sounds/Master Bank.strings.bank",
		"data/music/Music.bank"
	}};
	std::array<FMOD::Studio::Bank*, 3> banks{};
	for(int i = 0; i < (int)banks.size(); i++)
	{
		if(system->loadBankFile(bankNames[i].data(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[i]) != FMOD_OK)
		{
			return 1;
		}
		int count = 0;
		banks[i]->getEventCount(&count);
		std::vector<FMOD::Studio::EventDescription*> eventDescriptions(count);
		banks[i]->getEventList(eventDescriptions.data(), count, &count);

		for(auto* eventDescription: eventDescriptions)
		{
			char path[30];
			int retrieved = 0;
			eventDescription->getPath(path, 30, &retrieved);
			FMOD_GUID guid{};
			eventDescription->getID(&guid);
			std::cout << "Event path: "<< path << ", GUID: "<< guid.Data1 <<'-'<<guid.Data2<<'-'<<guid.Data3<<'\n';
		}
	}


	FMOD::Studio::EventDescription* eventDescription = NULL;
	if(system->getEvent("event:/Music/Music", &eventDescription ) != FMOD_OK)
	{
		std::terminate();
	}

	FMOD::Studio::EventInstance* eventInstance = NULL;
	if(eventDescription->createInstance(&eventInstance))
	{
		std::terminate();
	}

	eventInstance->start();
/* Initialises data */
	SDL_Window *window = NULL;

/*
* Initialises the SDL video subsystem (as well as the events subsystem).
* Returns 0 on success or a negative error code on failure using SDL_GetError().
*/
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return 1;
	}

/* Creates a SDL window */
	window = SDL_CreateWindow("SDL Example", /* Title of the SDL window */
			SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
			SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
			800, /* Width of the window in pixels */
			600, /* Height of the window in pixels */
			0); /* Additional flag(s) */

/* Checks if window has been created; if not, exits program */
	if (window == nullptr)
	{
		fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

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
		system->update();
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);
	}
	system->release();
	return 0;
}