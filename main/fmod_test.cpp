#include "engine/engine.h"
#include "engine/system.h"
#include "graphics/gui_renderer.h"
#include "audio/player_sound.h"
#include "audio/audio_manager.h"

#include <fmod_studio.hpp>
#include <SDL_main.h>
#include <imgui.h>

#include <iostream>

namespace splash
{
class FmodTestSystem : public SystemInterface, public OnGuiInterface
{
public:
	FmodTestSystem()
	{
		AddSystem(this);
		AddGuiInterface(this);
	}
	void Begin() override
	{

	}
	void End() override
	{
		RemoveSystem(this);
		RemoveGuiInterface(this);

	}
	void Update([[maybe_unused]] float dt) override
	{

	}
	[[nodiscard]] int GetSystemIndex() const override
	{
		return engineIndex_;
	}
	void SetSystemIndex(int index) override
	{
		engineIndex_ = index;
	}
	void OnGui() override
	{
		ImGui::Begin("Fmod Test Window");

		if(IsFmodLoaded())
		{
			if(ImGui::Button("Play Gun Sound"))
			{
				FMOD::Studio::EventDescription* eventDescription = GetEventDescription(GetPlayerSoundEvent(PlayerSoundId::GUN).data());

				FMOD::Studio::EventInstance* eventInstance = nullptr;
				if(eventDescription->createInstance(&eventInstance))
				{
					std::terminate();
				}
				eventInstance->start();
			}
		}
		else
		{
			ImGui::Text("Loading...");
		}
		ImGui::End();
	}
	void SetGuiIndex(int index) override
	{
		guiIndex_ = index;
	}
	int GetGuiIndex() const override
	{
		return guiIndex_;
	}
private:

	int engineIndex_ = -1;
	int guiIndex_ = -1;

};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::AudioManager audioManager;
	splash::FmodTestSystem testSystem;

	splash::RunEngine();
	return 0;
}