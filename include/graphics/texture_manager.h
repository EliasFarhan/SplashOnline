//
// Created by unite on 11.07.2024.
//

#ifndef SPLASHONLINE_TEXTURE_MANAGER_H
#define SPLASHONLINE_TEXTURE_MANAGER_H

#include <SDL_surface.h>
#include <SDL_render.h>

#include <array>

namespace splash
{

class TextureManager
{
public:
    void Begin();

	void End();

	[[nodiscard]] bool IsLoaded() const;
	void UpdateLoad();

	enum class TextureId
	{
		BACKFOG,
		BG,
		CLOUD1,
		CLOUD2,
		CLOUD3,
		FLOATROCK1,
		FLOATROCK2,
		FLOATROCK3,
		FLOATROCK4,
		FOG,
		FRONTFOG,
		LEAFBLOW_PARTICULE,
		MIDBG,
		PLAT1,
		PLAT2,
		PLAT3,
		LENGTH
	};

	[[nodiscard]] SDL_Texture* GetTexture(TextureId textureId) const noexcept{ return textures_[(int)textureId]; }
private:
	SDL_Renderer* renderer_ = nullptr;
	std::array<SDL_Texture*, (int)TextureId::LENGTH> textures_{};
};

}

#endif //SPLASHONLINE_TEXTURE_MANAGER_H
