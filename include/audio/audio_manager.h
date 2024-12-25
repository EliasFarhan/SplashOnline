
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
private:
	int systemIndex_ = 0;
};

FMOD::Studio::EventDescription* GetEventDescription(std::string_view eventName);
bool IsFmodLoaded();
FMOD::Studio::EventInstance* FmodPlaySound(std::string_view eventName);

}

#endif //SPLASHONLINE_AUDIO_MANAGER_H
