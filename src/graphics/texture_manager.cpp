//
// Created by unite on 11.07.2024.
//

//#define STB_IMAGE_IMPLEMENTATION //already defined in spine-sdl


#include "graphics/texture_manager.h"
#include "engine/engine.h"
#include "utils/log.h"

#include <thread/job_system.h>
#include <stb_image.h>
#include <fmt/format.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

#include <string_view>
#include <array>
#include <atomic>

namespace splash
{

struct Image
{
    void* pixels = nullptr;
    int width = 0;
    int height = 0;
    int comp = 0;
};
namespace
{

constexpr std::array<std::string_view, static_cast<int>(TextureManager::TextureId::LENGTH)> texturePaths =
    {{
         "data/sprites/kittymanjaro/bg.png",
         "data/sprites/kittymanjaro/cloud1.png",
         "data/sprites/kittymanjaro/cloud2.png",
         "data/sprites/kittymanjaro/cloud3.png",
         "data/sprites/kittymanjaro/floatrock1.png",
         "data/sprites/kittymanjaro/floatrock2.png",
         "data/sprites/kittymanjaro/floatrock3.png",
         "data/sprites/kittymanjaro/floatrock4.png",
         "data/sprites/kittymanjaro/backfog.png",
         "data/sprites/kittymanjaro/frontfog.png",
         "data/sprites/kittymanjaro/midbg.png",
         "data/sprites/kittymanjaro/plat1.png",
         "data/sprites/kittymanjaro/plat2.png",
         "data/sprites/kittymanjaro/plat3.png",

         "data/sprites/ui/arrow_cyan.png",
         "data/sprites/ui/arrow_orange.png",
         "data/sprites/ui/arrow_magenta.png",
         "data/sprites/ui/arrow_turquoise.png",
         "data/sprites/ui/right_arrow.png",
         "data/sprites/ui/cat_playerhead.png",
         "data/sprites/ui/lucha_playerhead.png",
         "data/sprites/ui/owl_playerhead.png",
         "data/sprites/ui/robo_playerhead.png",
         "data/sprites/ui/OOB_cyan.png",
         "data/sprites/ui/OOB_orange.png",
         "data/sprites/ui/OOB_mag.png",
         "data/sprites/ui/OOB_turq.png",
         "data/sprites/ui/cat_win.png",
         "data/sprites/ui/lucha_win.png",
         "data/sprites/ui/hoot_win.png",
         "data/sprites/ui/robo_win.png",
         "data/sprites/ui/cat_lost.png",
         "data/sprites/ui/lucha_lost.png",
         "data/sprites/ui/hoot_lost.png",
         "data/sprites/ui/robo_lost.png",
         "data/sprites/ui/switch_procontroller.png",

         "data/spine/kwakwalogo/SBPkwakwa_ver4_2.png",
         "data/spine/announcer/SBPannouncers_ver4_2.png",
         "data/spine/chars/SBPChars_ver4_2.png",
         "data/spine/arms/SBPCharArms_ver4_2.png",
         "data/spine/gun/SBPweapons_ver4_2.png",
         "data/spine/water/SBPwater_ver4_2.png",
         "data/spine/cloud/SBPCloud_ver_4_2.png",
         "data/spine/FX/SBPfx_ver4_2.png",



     }};
    std::array<SDL_Texture*, static_cast<int>(TextureManager::TextureId::LENGTH)> textures_{};
    std::array<neko::Vec2i, static_cast<int>(TextureManager::TextureId::LENGTH)> texturesSizes_{};

std::atomic<int> loadingIndex{-1};
std::array<Image, static_cast<int>(TextureManager::TextureId::LENGTH)> images;
std::array<SDL_Surface*, static_cast<int>(TextureManager::TextureId::LENGTH)> surfaces;
std::unique_ptr<neko::FuncJob> loadingJob;
}




Image LoadImageFromFile(std::string_view path)
{
	Image image{};
	image.pixels = stbi_load(path.data(), &image.width, &image.height, &image.comp, 0);
	if(image.pixels == nullptr)
	{
		LogError(fmt::format("STBI could not load file: {}", path.data()));
	}
	return image;
}

SDL_Surface* CreateSurfaceFromImage(const Image& image)
{
	switch(image.comp)
	{
	case 4:
		return SDL_CreateRGBSurfaceWithFormatFrom(image.pixels,
			image.width,
			image.height,
			8,
			image.width * image.comp,
			SDL_PIXELFORMAT_RGBA32);
	case 3:
		return SDL_CreateRGBSurfaceWithFormatFrom(image.pixels,
			image.width,
			image.height,
			8,
			image.width * image.comp,
			SDL_PIXELFORMAT_RGB24);
	default:
	{
		LogError("Unsupported texture format, invalid comp count");
		break;
	}
	}
	return nullptr;
}
SDL_Texture* CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface)
{
	return SDL_CreateTextureFromSurface(renderer, surface);
}



void TextureManager::Begin()
{
	loadingJob = std::make_unique<neko::FuncJob>([](){
		for(int i = 0; i < static_cast<int>(texturePaths.size()); i++)
		{
#ifdef TRACY_ENABLE
			ZoneNamedN(loadingTexture, "Loading Image File", true);
			ZoneTextV(loadingTexture, texturePaths[i].data(), texturePaths[i].size());
#endif
#ifdef TRACY_ENABLE
			TracyCZoneN(imageLoad, "Load Image From File", true);
			TracyCZoneText(imageLoad, texturePaths[i].data(), texturePaths[i].size());
#endif
			images[i] = LoadImageFromFile(texturePaths[i]);
#ifdef TRACY_ENABLE
			TracyCZoneEnd(imageLoad);
			TracyCZoneN(surfaceCreate, "Create Surface From Image", true);
			TracyCZoneText(surfaceCreate, texturePaths[i].data(), texturePaths[i].size());
#endif
			surfaces[i] = CreateSurfaceFromImage(images[i]);
#ifdef TRACY_ENABLE
			TracyCZoneEnd(surfaceCreate);
#endif
			loadingIndex.store(i, std::memory_order_release);
		}
	});
	ScheduleAsyncJob(loadingJob.get());
}

void TextureManager::End()
{
	for(auto& texture: textures_)
	{
		SDL_DestroyTexture(texture);
	}
	textures_ = {};
}

bool IsTexturesLoaded()
{
	return textures_[static_cast<int>(TextureManager::TextureId::LENGTH)-1] != nullptr;
}

void UpdateTexturesLoad()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(IsTexturesLoaded())
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
	for(int i = 0; i <= loadingIndex.load(std::memory_order_acquire); i++)
	{
		if(textures_[i] != nullptr) continue;
#ifdef TRACY_ENABLE
		ZoneNamedN(loadingTexture, "Loading SDL Texture", true);
		ZoneTextV(loadingTexture, texturePaths[i].data(), texturePaths[i].size());
#endif
		textures_[i] = CreateTextureFromSurface(GetRenderer(), surfaces[i]);
		texturesSizes_[i] = {images[i].width, images[i].height};
		SDL_FreeSurface(surfaces[i]);
		stbi_image_free(images[i].pixels);

		current = SDL_GetPerformanceCounter();
		delta =  static_cast<double>(current - GetCurrentFrameTime());

		if( static_cast<float>(delta/freq) > 0.01f)
		{
			break;
		}
	}
}
neko::Vec2i GetTextureSize(TextureManager::TextureId id)
{
	return texturesSizes_[static_cast<int>(id)];
}
SDL_Texture* GetTexture(TextureManager::TextureId textureId) noexcept
{
    return textures_[static_cast<std::size_t>(textureId)];
}
}