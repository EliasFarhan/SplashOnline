//
// Created by unite on 02.08.2024.
//

#include "graphics/spine_manager.h"
#include "graphics/texture_manager.h"
#include "utils/log.h"
#include "engine/engine.h"

#include <spine-sdl-cpp.h>
#include <fmt/format.h>

#include <array>
#include <string_view>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

namespace splash
{

namespace
{


static constexpr std::array<std::string_view, static_cast<int>(SpineManager::AtlasId::LENGTH)> atlasPaths
	{{
		 "data/spine/kwakwalogo/SBPkwakwa_ver4_2.atlas",
		 "data/spine/announcer/SBPannouncers_ver4_2.atlas",
		 "data/spine/chars/SBPChars_ver4_2.atlas",
		 "data/spine/arms/SBPCharArms_ver4_2.atlas",
		 "data/spine/gun/SBPweapons_ver4_2.atlas",
		 "data/spine/water/SBPwater_ver4_2.atlas",
		 "data/spine/cloud/SBPCloud_ver_4_2.atlas",
		 "data/spine/FX/SBPfx_ver4_2.atlas",
	 }};
static constexpr std::array<std::string_view, static_cast<int>(SpineManager::SkeletonId::LENGTH)> skeletonPaths
	{{
		 "data/spine/kwakwalogo/SBP_kwakwalogo.skel",
		 "data/spine/announcer/FX_announcers.skel",

		 "data/spine/chars/cat_noarm.skel",
		 "data/spine/chars/lucha_noarm.skel",
		 "data/spine/chars/owl_noarm.skel",
		 "data/spine/chars/robo_noarm.skel",

		 "data/spine/arms/cat_arm.skel",
		 "data/spine/arms/lucha_arm.skel",
		 "data/spine/arms/owl_arm.skel",
		 "data/spine/arms/robot_arm.skel",

		 "data/spine/gun/basegun_equip.skel",
		 "data/spine/water/WATER_normaldrops.skel",
		 "data/spine/cloud/cloud.skel",

		 "data/spine/FX/fx_cloudeject.skel",
		 "data/spine/FX/fx_cloudeject_explosion.skel",
		 "data/spine/FX/fx_dash.skel",
		 "data/spine/FX/fx_dashprep.skel",
		 "data/spine/FX/fx_eject.skel",
		 "data/spine/FX/fx_impact.skel",
		 "data/spine/FX/fx_impact2.skel",
		 "data/spine/FX/fx_impacthard.skel",
		 "data/spine/FX/fx_jetboom.skel",
		 "data/spine/FX/fx_jetpack1.skel",
		 "data/spine/FX/fx_jetpack2.skel",
		 "data/spine/FX/fx_landing.skel",
		 "data/spine/FX/fx_reload.skel",
		 "data/spine/FX/fx_star.skel",


	 }};

std::array<std::unique_ptr<spine::Atlas>, static_cast<int>(SpineManager::AtlasId::LENGTH)> atlases_{};
std::array<std::unique_ptr<spine::AtlasAttachmentLoader>, static_cast<int>(SpineManager::AtlasId::LENGTH)> attachmentLoaders_;
std::array<std::unique_ptr<spine::SkeletonJson>, static_cast<int>(SpineManager::SkeletonId::LENGTH)> skeletonJsons_;
std::array<spine::SkeletonData*, static_cast<int>(SpineManager::SkeletonId::LENGTH)> skeletonData_{};

class SpineLoader : public spine::TextureLoader
{
public:
	void load([[maybe_unused]] spine::AtlasPage& page, [[maybe_unused]] const spine::String& path)
	{
		static int i = 0;
		auto* texture = GetTexture(static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::KWAKWA_LOGO)+i));
		page.texture = texture;
		SDL_QueryTexture(texture, nullptr, nullptr, &page.width, &page.height);
		switch (page.magFilter) {
		case spine::TextureFilter_Nearest:
			SDL_SetTextureScaleMode(texture, SDL_ScaleModeNearest);
			break;
		case spine::TextureFilter_Linear:
			SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
			break;
		default:
			SDL_SetTextureScaleMode(texture, SDL_ScaleModeBest);
		}
		i++;
	}

	void unload([[maybe_unused]]void* texture)
	{
		//No need for this function, we are taking care of freeing our own textures
	}
};
SpineLoader loader;
}


void SpineManager::UpdateLoad()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(!IsTexturesLoaded())
	{
		return;
	}
	auto freq = static_cast<double>(SDL_GetPerformanceFrequency());
	auto current = SDL_GetPerformanceCounter();
	auto delta =  static_cast<double>(current - GetCurrentFrameTime());
	if( static_cast<float>(delta/freq) > 0.01f)
	{
		return;
	}
	for(int i = 0; i < static_cast<int>(AtlasId::LENGTH); i++)
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(loadingAtlas, "Loading Atlas", true);
#endif
		if(atlases_[i] != nullptr)
			continue;
		atlases_[i] = std::make_unique<spine::Atlas>(atlasPaths[i].data(), &loader);
		if(atlases_[i]->getPages().size() == 0)
		{
			LogError(fmt::format("Could not load atlas data: {}", atlasPaths[i]));
		}
		attachmentLoaders_[i] = std::make_unique<spine::AtlasAttachmentLoader>(atlases_[i].get());

		current = SDL_GetPerformanceCounter();
		delta =  static_cast<double>(current - GetCurrentFrameTime());

		if( static_cast<float>(delta/freq) > 0.01f)
		{
			return;
		}
	}
	int j = 0;
	for(int i = 0; i < static_cast<int>(SkeletonId::LENGTH); i++)
	{
		if(atlases_[j] == nullptr)
		{
			break;
		}
		if(skeletonJsons_[i] != nullptr || skeletonData_[i] != nullptr)
		{
			switch(static_cast<SkeletonId>(i))
			{
			case SkeletonId::CAT_NOARM:
			case SkeletonId::LUCHA_NOARM:
			case SkeletonId::OWL_NOARM:
			case SkeletonId::CAT_ARM:
			case SkeletonId::LUCHA_ARM:
			case SkeletonId::OWL_ARM:
				continue;
			default:
				if(j < static_cast<int>(AtlasId::LENGTH)-1)
				{
					j++;
				}
				continue;
			}
		}
#ifdef TRACY_ENABLE
		ZoneNamedN(loadingSkeleton, "Loading Skeleton Data", true);
#endif
#ifdef TRACY_ENABLE
		TracyCZoneN(createSkeletonJson, "Create Skeleton Json", true);
#endif
		skeletonJsons_[i] = std::make_unique<spine::SkeletonJson>(attachmentLoaders_[j].get());
#ifdef TRACY_ENABLE
		TracyCZoneEnd(createSkeletonJson);
#endif
#ifdef TRACY_ENABLE
		TracyCZoneN(createSkeletonData, "Create Skeleton Data", true);
#endif
		skeletonData_[i] = skeletonJsons_[i]->readSkeletonDataFile(skeletonPaths[i].data());
#ifdef TRACY_ENABLE
		TracyCZoneEnd(createSkeletonData);
#endif
		if(skeletonData_[i] == nullptr)
		{
			LogError(fmt::format("Could not load skeleton data: {}", skeletonPaths[i]));
		}

		switch(static_cast<SkeletonId>(i))
		{
		case SkeletonId::CAT_NOARM:
		case SkeletonId::LUCHA_NOARM:
		case SkeletonId::OWL_NOARM:
		case SkeletonId::CAT_ARM:
		case SkeletonId::LUCHA_ARM:
		case SkeletonId::OWL_ARM:
			break;
		default:
			if(j < static_cast<int>(AtlasId::LENGTH)-1)
			{
				j++;
			}
			break;
		}
		current = SDL_GetPerformanceCounter();
		delta =  static_cast<double>(current - GetCurrentFrameTime());

		if( static_cast<float>(delta/freq) > 0.01f)
		{
			break;
		}
	}
}
void SpineManager::Begin()
{

}
void SpineManager::Update([[maybe_unused]]float dt)
{
	UpdateLoad();
}
void SpineManager::End()
{
	for(auto* skeletonData: skeletonData_)
	{
		delete skeletonData;
		skeletonData = nullptr;
	}
	atlases_ = {};
	attachmentLoaders_ = {};
	skeletonJsons_ = {};
}
bool IsSpineLoaded()
{
	return atlases_[static_cast<int>(SpineManager::AtlasId::LENGTH)-1] != nullptr && skeletonData_[static_cast<int>(SpineManager::SkeletonId::LENGTH)-1] != nullptr;
}
std::unique_ptr<spine::SkeletonDrawable> CreateSkeletonDrawable(SpineManager::SkeletonId skeletonId)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(skeletonData_[static_cast<int>(skeletonId)] == nullptr)
	{
		LogError(fmt::format("Could not load spine skeleton drawable: {}", static_cast<int>(skeletonId)));
		return nullptr;
	}
	return std::make_unique<spine::SkeletonDrawable>(skeletonData_[static_cast<int>(skeletonId)]);
}
}