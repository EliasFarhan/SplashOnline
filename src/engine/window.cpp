#include "engine/window.h"

#include <iostream>

namespace splash
{

#define WIDTH 800
#define HEIGHT 600

void Window::Begin()
{
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
		fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
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
    SDL_Event e;
    //Handle events on queue
    while (SDL_PollEvent(&e) != 0)
    {
        //User requests quit
        if (e.type == SDL_QUIT)
        {
            isOpen_ = false;
        }
        //TODO do something for event
        for(auto* eventInterface : eventInterfaces_)
        {
            eventInterface->OnEvent(e);
        }
    }
}

void Window::AddEventListener(OnEventInterface *eventInterface)
{
    eventInterfaces_.push_back(eventInterface);
}
bool Window::IsOpen() const noexcept
{
	return isOpen_;
}

SDL_Window* GetWindow()
{
	return instance->GetWindow();
}

void AddEventListener(OnEventInterface *eventInterface)
{
    instance->AddEventListener(eventInterface);
}
}