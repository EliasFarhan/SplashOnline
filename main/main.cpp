#include "graphics/texture_manager.h"

#include <SDL.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600
#define DELAY 3000

int main(int argc, char** argv)
{
/* Initialises data */
	SDL_Window* window = NULL;

/*
* Initialises the SDL video subsystem (as well as the events subsystem).
* Returns 0 on success or a negative error code on failure using SDL_GetError().
*/
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return 1;
	}

/* Creates a SDL window */
	window = SDL_CreateWindow("SDL Example", /* Title of the SDL window */
		SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
		SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
		WIDTH, /* Width of the window in pixels */
		HEIGHT, /* Height of the window in pixels */
		0); /* Additional flag(s) */

/* Checks if window has been created; if not, exits program */
	if (window == nullptr)
	{
		fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer == nullptr)
	{
		fprintf(stderr, "SDL renderer failed to initialise: %s\n", SDL_GetError());

		return 1;
	}
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);

	std::cout << "SDL Renderer name: " << info.name << '\n';

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

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
			ImGui_ImplSDL2_ProcessEvent(&e);
		}


		//Clear screen
		SDL_RenderClear(renderer);


		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Splash Online");
		ImGui::End();
		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(renderer,
			io.DisplayFramebufferScale.x,
			io.DisplayFramebufferScale.y);
		//Update screen
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	/* Frees memory */
	SDL_DestroyWindow(window);

	/* Shuts down all SDL subsystems */
	SDL_Quit();

	return 0;
}