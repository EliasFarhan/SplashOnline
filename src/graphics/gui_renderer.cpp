//
// Created by unite on 12.07.2024.
//

#include "graphics/gui_renderer.h"
#include "graphics/graphics_manager.h"
#include "engine/window.h"

#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_impl_sdl2.h>

namespace splash
{

void GuiRenderer::Begin()
{
	renderer_ = GetRenderer();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(GetWindow(), renderer_);
	ImGui_ImplSDLRenderer2_Init(renderer_);
}

void GuiRenderer::End()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void GuiRenderer::Draw()
{
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	SDL_RenderSetScale(renderer_,
		io.DisplayFramebufferScale.x,
		io.DisplayFramebufferScale.y);
	//Update screen
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);
}

void GuiRenderer::AddListener(OnGuiInterface* guiInterface)
{
	guiInterfaces_.push_back(guiInterface);
}
}