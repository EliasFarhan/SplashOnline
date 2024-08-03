#include "audio/audio_manager.h"
#include "engine/engine.h"

#include <string_view>
#include <array>

namespace splash
{
static AudioManager* instance = nullptr;

void AudioManager::Begin()
{
	if(FMOD::Studio::System::create(&system_) != FMOD_OK)
	{
		std::terminate();
	}
	if(system_->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr))
	{
		std::terminate();
	}
	static constexpr std::array<std::string_view, 3> bankNames =
		{{
			 "data/sounds/Master Bank.bank",
			 "data/sounds/Master Bank.strings.bank",
			 "data/music/Music.bank"
		 }};
	std::array<FMOD::Studio::Bank*, 3> banks{};
	for(int i = 0; i < banks.size(); i++)
	{
		if(system_->loadBankFile(bankNames[i].data(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[i]) != FMOD_OK)
		{
			std::terminate();
		}
	}
	musicManager_.Begin();
}
void AudioManager::End()
{
	RemoveSystem(this);
	musicManager_.End();
	system_->release();
}
void AudioManager::Update([[maybe_unused]]float dt)
{
	system_->update();
}
int AudioManager::GetSystemIndex() const
{
	return systemIndex_;
}
void AudioManager::SetSystemIndex(int index)
{
	systemIndex_ = index;
}
FMOD::Studio::EventDescription* AudioManager::GetEventDescription(std::string_view eventName)
{
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	if(system_->getEvent(eventName.data(), &eventDescription ) != FMOD_OK)
	{
		return nullptr;
	}
	return eventDescription;
}
AudioManager::AudioManager()
{
	instance = this;
	AddSystem(this);
}
FMOD::Studio::EventDescription* GetEventDescription(std::string_view eventName)
{
	return instance->GetEventDescription(eventName);
}
}