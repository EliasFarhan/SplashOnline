//
// Created by unite on 08.08.2024.
//

#ifndef SPLASHONLINE_INCLUDE_GAME_GRAPHICS_LEVEL_RENDERER_H_
#define SPLASHONLINE_INCLUDE_GAME_GRAPHICS_LEVEL_RENDERER_H_

#include <math/vec2.h>
#include <SDL_render.h>

#include <array>

namespace splash
{
class LevelView
{
public:
	void Begin();
	void Update(float dt);
	void End();
	void Draw();
private:

	struct Rock
	{
		SDL_Texture* texture;
		neko::Vec2i textureSize{};
		float amplitude{};
		float floatingPeriod{};
		float floatingTime{};
	};
	struct Cloud
	{
		float currentPosX{};
		float speedX{};
		static constexpr float maxDist = 12.0f;
	};
	void LoadTextures();
	std::array<SDL_Texture*, 3> platformTextures_{};
	std::array<SDL_Texture*, 2> fogTextures_{};
	std::array<float, 2> fogOffsets_{};
	SDL_Texture* backgroundTexture_{};
	SDL_Texture* midBgTexture_{};
	std::array<Rock, 4> rocks_{};
	std::array<Cloud, 6> clouds_{};
};
}
#endif //SPLASHONLINE_INCLUDE_GAME_GRAPHICS_LEVEL_RENDERER_H_
