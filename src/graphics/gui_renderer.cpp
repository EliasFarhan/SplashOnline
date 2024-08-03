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
static GuiRenderer* instance = nullptr;


GuiRenderer::GuiRenderer()
{
	instance = this;
	AddEventListener(this);
}

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
	instance = nullptr;
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

void GuiRenderer::AddGuiInterface(OnGuiInterface* guiInterface)
{
	auto it = std::find(guiInterfaces_.begin(), guiInterfaces_.end(), nullptr);
	if(it != guiInterfaces_.end())
	{
		guiInterface->SetGuiIndex((int)std::distance(guiInterfaces_.begin(), it));
		*it = guiInterface;
	}
	else
	{
		guiInterface->SetGuiIndex((int)guiInterfaces_.size());
		guiInterfaces_.push_back(guiInterface);
	}
	guiInterfaces_.push_back(guiInterface);
}

void GuiRenderer::RemoveGuiInterface(OnGuiInterface* guiInterface)
{
	guiInterfaces_[guiInterface->GetGuiIndex()] = nullptr;
}
void GuiRenderer::OnEvent(const SDL_Event& event)
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}
void GuiRenderer::Update()
{
	// Start the Dear ImGui frame
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Splash Online");
	ImGui::End();
	for(auto* guiInterface : guiInterfaces_)
	{
		guiInterface->OnGui();
	}
}

int GuiRenderer::GetEventListenerIndex() const
{
	return eventListenerIndex_;
}

void GuiRenderer::SetEventListenerIndex(int index)
{
	eventListenerIndex_ = index;
}

void AddGuiInterface(OnGuiInterface* guiInterface)
{
	instance->AddGuiInterface(guiInterface);
}

void RemoveGuiInterface(OnGuiInterface* guiInterface)
{
	instance->RemoveGuiInterface(guiInterface);
}
}