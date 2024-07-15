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

SDL_Window* GetWindow()
{
	return instance->GetWindow();
}
}