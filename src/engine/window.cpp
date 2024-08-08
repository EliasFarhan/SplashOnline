#include "engine/window.h"
#include "utils/log.h"

#include <fmt/format.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

#define WIDTH 800
#define HEIGHT 600

void Window::Begin()
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
		0); /* Additional flag(s) */

/* Checks if window has been created; if not, exits program */
	if (window_ == nullptr)
	{
		LogError(fmt::format("SDL window failed to initialise: {}\n", SDL_GetError()));
		return;
	}
    isOpen_ = true;
}

void Window::End()
{
	/* Frees memory */
	SDL_DestroyWindow(window_);
}

static Window* instance = nullptr;
Window::Window()
{
	instance = this;
}

SDL_Window* Window::GetWindow()
{
	return window_;
}

void Window::Update()
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
        for(auto* eventInterface : eventInterfaces_)
        {
            eventInterface->OnEvent(e);
        }
    }
}

void Window::AddEventListener(OnEventInterface *eventInterface)
{
	auto it = std::find(eventInterfaces_.begin(), eventInterfaces_.end(), nullptr);
	if(it != eventInterfaces_.end())
	{
		eventInterface->SetEventListenerIndex((int)std::distance(eventInterfaces_.begin(), it));
		*it = eventInterface;
	}
	else
	{
		eventInterface->SetEventListenerIndex((int)eventInterfaces_.size());
		eventInterfaces_.push_back(eventInterface);
	}
}
bool Window::IsOpen() const noexcept
{
	return isOpen_;
}

std::pair<int, int> Window::GetWindowSize() const
{
	std::pair<int, int> windowSize;
	SDL_GetWindowSize(window_, &windowSize.first, &windowSize.second);
    return windowSize;
}

void Window::RemoveEventListener(OnEventInterface* eventInterface)
{
	eventInterfaces_[eventInterface->GetEventListenerIndex()] = nullptr;
}

SDL_Window* GetWindow()
{
	return instance->GetWindow();
}

void AddEventListener(OnEventInterface *eventInterface)
{
    instance->AddEventListener(eventInterface);
}

std::pair<int, int> GetWindowSize()
{
	return instance->GetWindowSize();
}
}