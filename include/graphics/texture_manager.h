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
	TextureManager();
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

		CAT_RIGHTARM,
		LUCHA_RIGHTARM,
		OWL_RIGHTARM,
		ROBOT_RIGHTARM,

		//Spine pages
		KWAKWA_LOGO,
		ANNOUCERS,
		CHARS_NOARM,
		CHARS_ARM,
		GUN,
		CLOUD_ANIM,
		FX,

		LENGTH
	};

	[[nodiscard]] SDL_Texture* GetTexture(TextureId textureId) const noexcept{ return textures_[(int)textureId]; }
private:
	SDL_Renderer* renderer_ = nullptr;
	std::array<SDL_Texture*, (int)TextureId::LENGTH> textures_{};
};

[[nodiscard]] SDL_Texture* GetTexture(TextureManager::TextureId textureId);
bool IsTextureLoaded();

}

#endif //SPLASHONLINE_TEXTURE_MANAGER_H
