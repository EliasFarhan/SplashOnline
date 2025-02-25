
#ifndef SPLASHONLINE_AUDIO_MANAGER_H
#define SPLASHONLINE_AUDIO_MANAGER_H


#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
#include <fmod_studio.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <string_view>

namespace splash
{

void AddAudio();
FMOD::Studio::EventDescription* GetEventDescription(std::string_view eventName);
bool IsFmodLoaded();
FMOD::Studio::EventInstance* FmodPlaySound(std::string_view eventName);

}

#endif //SPLASHONLINE_AUDIO_MANAGER_H
