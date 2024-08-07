//
// Created by unite on 02.08.2024.
//

#include "graphics/spine_manager.h"

#include <spine-sdl-cpp.h>
#include <array>
#include <string_view>

namespace splash
{

static constexpr std::array<std::string_view, (int)SpineManager::AtlasId::LENGTH> atlasPaths
{{
	"data/spine/kwakwa logo/SBP_kwakwa_ver4_2.atlas",
	"data/spine/announcer/SBPannouncers_ver4_2.atlas",
	"data/spine/chars/SBPChars_ver4_2.atlas",
	"data/spine/arms/SBPCharArms_ver4_2.atlas",
	"data/spine/cloud/SBPCloud_ver_4_2.atlas",
	"data/spine/gun/SBPweapons_ver4_2.atlas",
	"data/spine/FX/SBPfx_ver4_2.atlas",
}};

void SpineManager::load(spine::AtlasPage& page, const spine::String& path)
{

}

void SpineManager::unload([[maybe_unused]]void* texture)
{
	//No need for this function, we are taking care of freeing our own textures
}
}