//
// Created by unite on 08.08.2024.
//

#ifndef SPLASHONLINE_INCLUDE_GAME_GRAPHICS_LEVEL_RENDERER_H_
#define SPLASHONLINE_INCLUDE_GAME_GRAPHICS_LEVEL_RENDERER_H_

#include <SDL_render.h>

#include <array>

namespace splash
{
class LevelRenderer
{
public:
	void Begin();
	void Update();
	void End();
	void Draw();
private:
	void LoadTextures();
	std::array<SDL_Texture*, 3> platformTextures_{};
	SDL_Texture* backgroundTexture_{};
};
}
#endif //SPLASHONLINE_INCLUDE_GAME_GRAPHICS_LEVEL_RENDERER_H_
