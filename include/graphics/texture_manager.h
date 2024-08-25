//
// Created by unite on 11.07.2024.
//

#ifndef SPLASHONLINE_TEXTURE_MANAGER_H
#define SPLASHONLINE_TEXTURE_MANAGER_H

#include <SDL_surface.h>
#include <SDL_render.h>

#include <array>
#include "math/vec2.h"

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

		//UI
		ARROW_P1_CYAN,
		ARROW_P2_ORANGE,
		ARROW_P3_MAGENTA,
		ARROW_P4_TURQUOISE,
		RIGHT_ARROW,
		HEAD_P1_CAT,
		HEAD_P2_LUCHA,
		HEAD_P3_OWL,
		HEAD_P4_ROBO,
		OOB_P1_CYAN,
		OOB_P2_ORANGE,
		OOB_O3_MAGENTA,
		OOB_04_TURQUOISE,
		WIN_P1_CAT,
		WIN_P2_LUCHA,
		WIN_P3_OWL,
		WIN_P4_ROBO,
		LOSE_P1_CAT,
		LOSE_P2_LUCHA,
		LOSE_P3_OWL,
		LOSE_P4_ROBO,

		//Spine pages
		KWAKWA_LOGO,
		ANNOUCERS,
		CHARS_NOARM,
		CHARS_ARM,
		GUN,
		WATA,
		CLOUD_ANIM,
		FX,



		LENGTH
	};

	[[nodiscard]] SDL_Texture* GetTexture(TextureId textureId) const noexcept{ return textures_[(int)textureId]; }
	neko::Vec2i GetTextureSize(TextureId id) const;
private:
	SDL_Renderer* renderer_ = nullptr;
	std::array<SDL_Texture*, (int)TextureId::LENGTH> textures_{};
	std::array<neko::Vec2i, (int)TextureId::LENGTH> texturesSizes_{};
};

[[nodiscard]] SDL_Texture* GetTexture(TextureManager::TextureId textureId);
[[nodiscard]] neko::Vec2i GetTextureSize(TextureManager::TextureId textureId);
bool IsTextureLoaded();

}

#endif //SPLASHONLINE_TEXTURE_MANAGER_H
