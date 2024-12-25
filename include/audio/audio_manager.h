
#ifndef SPLASHONLINE_AUDIO_MANAGER_H
#define SPLASHONLINE_AUDIO_MANAGER_H

#include "engine/system.h"
#include "audio/music_manager.h"

#include <fmod_studio.hpp>

#include <string_view>
#include <atomic>

namespace splash
{

class AudioManager : public SystemInterface
{
public:
	AudioManager();
	void Begin() override;
	void End() override;
	void Update(float dt) override;
	[[nodiscard]] int GetSystemIndex() const override;
	void SetSystemIndex(int index) override;

	FMOD::Studio::EventDescription* GetEventDescription(std::string_view eventName);
	[[nodiscard]] bool IsLoaded() const { return isLoaded_.load(std::memory_order_consume); }

	FMOD::Studio::EventInstance* PlaySound(std::string_view eventName);
private:
	FMOD::Studio::System* system_ = nullptr;
	int systemIndex_ = 0;
	std::atomic<bool> isLoaded_{false};
};

FMOD::Studio::EventDescription* GetEventDescription(std::string_view eventName);
bool IsFmodLoaded();
FMOD::Studio::EventInstance* FmodPlaySound(std::string_view eventName);

}

#endif //SPLASHONLINE_AUDIO_MANAGER_H
