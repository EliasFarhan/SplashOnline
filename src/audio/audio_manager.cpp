#include "audio/audio_manager.h"
#include "engine/engine.h"
#include "utils/log.h"

#include <fmt/format.h>

#include <string_view>
#include <array>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

namespace splash
{
static AudioManager* instance = nullptr;

void AudioManager::Begin()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	static std::unique_ptr<neko::FuncJob> loadingJob = std::make_unique<neko::FuncJob>([this]()
	{
#ifdef TRACY_ENABLE
	  TracyCZoneN(systemCreate, "Create Fmod Studio System", true);
#endif
	  if(FMOD::Studio::System::create(&system_) != FMOD_OK)
	  {
		  std::terminate();
	  }
#ifdef TRACY_ENABLE
	  TracyCZoneEnd(systemCreate);
	  TracyCZoneN(systemInit, "Init Fmod Studio System", true);
#endif
	  if(system_->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr))
	  {
		  return;
	  }
#ifdef TRACY_ENABLE
	  TracyCZoneEnd(systemInit);
#endif
	  static constexpr std::array<std::string_view, 3> bankNames =
		  {{
			   "data/sounds/Master Bank.bank",
			   "data/sounds/Master Bank.strings.bank",
			   "data/music/Music.bank"
		   }};
	  std::array<FMOD::Studio::Bank*, 3> banks{};
	  for (std::size_t i = 0; i < banks.size(); i++)
	  {
#ifdef TRACY_ENABLE
		  ZoneNamedN(loadBank, "Load Bank", true);
#endif
		  if (system_->loadBankFile(bankNames[i].data(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[i]) != FMOD_OK)
		  {
			  std::terminate();
		  }
	  }
	  musicManager_.Begin();
	  isLoaded_.store(true, std::memory_order_release);
	});
	ScheduleAsyncJob(loadingJob.get());
}
void AudioManager::End()
{
	RemoveSystem(this);
	musicManager_.End();
	system_->release();
}
void AudioManager::Update([[maybe_unused]]float dt)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(isLoaded_.load(std::memory_order_consume))
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(updateFmodSystem, "Update Fmod", true);
#endif
		system_->update();
	}
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
	const auto errorCode = system_->getEvent(eventName.data(), &eventDescription );
	if(errorCode != FMOD_OK)
	{
		LogError(fmt::format("Fmod getting event description error: {}", static_cast<int>(errorCode)));
		return nullptr;
	}
	return eventDescription;
}
AudioManager::AudioManager()
{
	instance = this;
	AddSystem(this);
}
FMOD::Studio::EventInstance* AudioManager::PlaySound(std::string_view eventName)
{
	FMOD::Studio::EventDescription* eventDescription = GetEventDescription(eventName);

	if(eventDescription == nullptr)
	{
		return nullptr;
	}
	FMOD::Studio::EventInstance* eventInstance = nullptr;
	if(eventDescription->createInstance(&eventInstance))
	{
		LogError(fmt::format("Could not create instance for event: {}", eventName));
		return nullptr;
	}
	eventInstance->start();
	eventInstance->release();
	return eventInstance;
}
FMOD::Studio::EventDescription* GetEventDescription(std::string_view eventName)
{
	return instance->GetEventDescription(eventName);
}
bool IsFmodLoaded()
{
	return instance->IsLoaded();
}
MusicManager& GetMusicManager()
{
	return instance->GetMusicManager();
}
FMOD::Studio::EventInstance* FmodPlaySound(std::string_view eventName)
{
	return instance->PlaySound(eventName);
}
}