//
// Created by unite on 11.07.2024.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "graphics/texture_manager.h"

#include <string_view>
#include <array>
#include <thread>
#include <atomic>

namespace splash
{
static std::array<std::string_view, (int)TextureManager::TextureId::LENGTH> texturePaths =
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
static std::thread loadingThread;
static std::atomic<int> loadingIndex{-1};
static std::array<Image, (int)TextureManager::TextureId::LENGTH> images;
static std::array<SDL_Surface*, (int)TextureManager::TextureId::LENGTH> surfaces;
void TextureManager::Begin()
{
	loadingThread = std::thread([](){
		for(int i = 0; i < texturePaths.size(); i++)
		{
			images[i] = LoadImageFromFile(texturePaths[i]);
			surfaces[i] = CreateSurfaceFromImage(images[i]);
			loadingIndex.store(i, std::memory_order_release);
		}
	});
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
	if(IsLoaded())
	{
		if(loadingThread.joinable())
		{
			loadingThread.join();
		}
		return;
	}
	for(int i = 0; i <= loadingIndex.load(std::memory_order_consume); i++)
	{
		if(textures_[i] != nullptr) continue;
		textures_[i] = CreateTextureFromSurface(renderer_, surfaces[i]);
	}
}
}