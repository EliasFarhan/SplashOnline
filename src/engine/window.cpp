#include "engine/window.h"
#include "utils/log.h"
#include "engine/input_manager.h"
#include "graphics/graphics_manager.h"

#include <fmt/format.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{
namespace
{
    std::vector<OnEventInterface*> eventInterfaces_;
    SDL_Window* window_ = nullptr;
    bool isOpen_ = false;
}

#define WIDTH 1280
#define HEIGHT 720

void BeginWindow()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
/* Creates a SDL window */
	window_ = SDL_CreateWindow("Splash Online", /* Title of the SDL window */
		SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
		SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
		WIDTH, /* Width of the window in pixels */
		HEIGHT, /* Height of the window in pixels */
		SDL_WINDOW_RESIZABLE ); /* Additional flag(s) */

/* Checks if window has been created; if not, exits program */
	if (window_ == nullptr)
	{
		LogError(fmt::format("SDL window failed to initialise: {}\n", SDL_GetError()));
		std::terminate();
	}
    isOpen_ = true;
}

void EndWindow()
{
	/* Frees memory */
	SDL_DestroyWindow(window_);
}


SDL_Window* GetWindow()
{
	return window_;
}

void UpdateWindow()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
    SDL_Event e;
    //Handle events on queue
    while (SDL_PollEvent(&e) != 0)
    {
        //User requests quit
        if (e.type == SDL_QUIT)
        {
            isOpen_ = false;
        }
        ManageInputEvent(e);
        ManageGraphicsEvent(e);
        for(auto* eventInterface : eventInterfaces_)
        {
            eventInterface->OnEvent(e);
        }
    }
}

void AddEventListener(OnEventInterface *eventInterface)
{
	auto it = std::find(eventInterfaces_.begin(), eventInterfaces_.end(), nullptr);
	if(it != eventInterfaces_.end())
	{
		eventInterface->SetEventListenerIndex(static_cast<int>(std::distance(eventInterfaces_.begin(), it)));
		*it = eventInterface;
	}
	else
	{
		eventInterface->SetEventListenerIndex(static_cast<int>(eventInterfaces_.size()));
		eventInterfaces_.push_back(eventInterface);
	}
}
bool IsWindowOpen()
{
	return isOpen_;
}

std::pair<int, int> GetWindowSize()
{
	std::pair<int, int> windowSize;
	SDL_GetWindowSize(window_, &windowSize.first, &windowSize.second);
    return windowSize;
}

void RemoveEventListener(OnEventInterface* eventInterface)
{
	eventInterfaces_[eventInterface->GetEventListenerIndex()] = nullptr;
}

}