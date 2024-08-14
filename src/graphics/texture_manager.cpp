//
// Created by unite on 11.07.2024.
//

//#define STB_IMAGE_IMPLEMENTATION //already defined in spine-sdl
#include <stb_image.h>

#include "graphics/texture_manager.h"
#include "engine/engine.h"

#include <thread/job_system.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

#include <string_view>
#include <array>
#include <atomic>

namespace splash
{

static TextureManager* instance = nullptr;

static constexpr std::array<std::string_view, (int)TextureManager::TextureId::LENGTH> texturePaths =
	{{
		"data/sprites/kittymanjaro/backfog.png",
		"data/sprites/kittymanjaro/bg.png",
		"data/sprites/kittymanjaro/cloud1.png",
		"data/sprites/kittymanjaro/cloud2.png",
		"data/sprites/kittymanjaro/cloud3.png",
		"data/sprites/kittymanjaro/floatrock1.png",
		"data/sprites/kittymanjaro/floatrock2.png",
		"data/sprites/kittymanjaro/floatrock3.png",
		"data/sprites/kittymanjaro/floatrock4.png",
		"data/sprites/kittymanjaro/fog.png",
		"data/sprites/kittymanjaro/frongfog.png",
		"data/sprites/kittymanjaro/leafblow_particle.png",
		"data/sprites/kittymanjaro/midbg.png",
		"data/sprites/kittymanjaro/plat1.png",
		"data/sprites/kittymanjaro/plat2.png",
		"data/sprites/kittymanjaro/plat3.png",


		"data/spine/kwakwalogo/SBPkwakwa_ver4_2.png",
		"data/spine/announcer/SBPannouncers_ver4_2.png",
		"data/spine/chars/SBPChars_ver4_2.png",
		"data/spine/arms/SBPCharArms_ver4_2.png",
		"data/spine/gun/SBPweapons_ver4_2.png",
		"data/spine/cloud/SBPCloud_ver_4_2.png",
		"data/spine/FX/SBPfx_ver4_2.png",

	}};

struct Image
{
	void* pixels = nullptr;
	int width = 0;
	int height = 0;
	int comp = 0;
};

Image LoadImageFromFile(std::string_view path)
{
	Image image{};
	image.pixels = stbi_load(path.data(), &image.width, &image.height, &image.comp, 0);
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
		break;
	}
	return nullptr;
}
SDL_Texture* CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface)
{
	return SDL_CreateTextureFromSurface(renderer, surface);
}

static std::atomic<int> loadingIndex{-1};
static std::array<Image, (int)TextureManager::TextureId::LENGTH> images;
static std::array<SDL_Surface*, (int)TextureManager::TextureId::LENGTH> surfaces;
static std::unique_ptr<neko::FuncJob> loadingJob;

void TextureManager::Begin()
{
	renderer_ = GetRenderer();
	loadingJob = std::make_unique<neko::FuncJob>([](){
		for(int i = 0; i < (int)texturePaths.size(); i++)
		{
#ifdef TRACY_ENABLE
			ZoneNamedN(loadingTexture, "Loading Image File", true);
#endif
#ifdef TRACY_ENABLE
			TracyCZoneN(imageLoad, "Load Image From File", true);
#endif
			images[i] = LoadImageFromFile(texturePaths[i]);
#ifdef TRACY_ENABLE
			TracyCZoneEnd(imageLoad);
			TracyCZoneN(surfaceCreate, "Create Surface From Image", true);
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

bool TextureManager::IsLoaded() const
{
	return textures_[(int)TextureManager::TextureId::LENGTH-1] != nullptr;
}

void TextureManager::UpdateLoad()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(IsLoaded())
	{
		return;
	}
	auto freq = (double)SDL_GetPerformanceFrequency();
	auto current = SDL_GetPerformanceCounter();
	auto delta =  (double)(current - GetCurrentFrameTime());
	if( (float)(delta/freq) > 0.01f)
	{
		return;
	}
	for(int i = 0; i <= loadingIndex.load(std::memory_order_consume); i++)
	{
		if(textures_[i] != nullptr) continue;
#ifdef TRACY_ENABLE
		ZoneNamedN(loadingTexture, "Loading SDL Texture", true);
#endif
		textures_[i] = CreateTextureFromSurface(renderer_, surfaces[i]);
		SDL_FreeSurface(surfaces[i]);
		stbi_image_free(images[i].pixels);

		current = SDL_GetPerformanceCounter();
		delta =  (double)(current - GetCurrentFrameTime());

		if( (float)(delta/freq) > 0.01f)
		{
			break;
		}
	}
}
TextureManager::TextureManager()
{
	instance = this;
}
SDL_Texture* GetTexture(TextureManager::TextureId textureId)
{
	return instance->GetTexture(textureId);
}
bool IsTextureLoaded()
{
	return instance->IsLoaded();
}
}