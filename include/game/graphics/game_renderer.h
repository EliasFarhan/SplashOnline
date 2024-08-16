//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_RENDERER_H_
#define SPLASHONLINE_GAME_RENDERER_H_

#include "graphics/graphics_manager.h"
#include "game/graphics/player_renderer.h"
#include "game/graphics/bullet_renderer.h"
#include "game/game_systems.h"
#include "game/graphics/level_renderer.h"

namespace splash
{

class GameRenderer : public DrawInterface
{
public:
	explicit GameRenderer(const GameSystems* gameSystems);
	void Begin();
	void Update(float dt);
	void Tick();
	void End();
	void Draw() override;
	void SetGraphicsIndex(int index) override;
	[[nodiscard]] int GetGraphicsIndex() const override;
	[[nodiscard]] float GetTimeSinceTick() const;

private:
	PlayerRenderer playerRenderer_;
	BulletRenderer bulletRenderer_;
	LevelRenderer levelRenderer_;
	int graphicIndex = -1;
	float timeSinceTick_ = 0.0f;
};

float GetTimeSinceTick();
}
#endif //SPLASHONLINE_GAME_RENDERER_H_
