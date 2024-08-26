//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_RENDERER_H_
#define SPLASHONLINE_GAME_RENDERER_H_

#include "graphics/graphics_manager.h"
#include "game/graphics/player_view.h"
#include "game/graphics/bullet_view.h"
#include "game/game_systems.h"
#include "game/graphics/level_view.h"
#include "game/graphics/end_screen_view.h"

namespace splash
{

class GameView : public DrawInterface
{
public:
	explicit GameView(const GameSystems* gameSystems);
	void Begin();
	void Update(float dt);
	void Tick();
	void End();
	void Draw() override;
	void SetGraphicsIndex(int index) override;
	[[nodiscard]] int GetGraphicsIndex() const override;
	[[nodiscard]] float GetTimeSinceTick() const;

	EndScreenView& GetEndScreenView() {return endScreenView_;}

	void SetGameOver(bool isGameOver){isGameOver_ = isGameOver;}
	[[nodiscard]] bool IsGameOver() const { return isGameOver_;}
private:
	PlayerView playerRenderer_;
	BulletView bulletRenderer_;
	LevelView levelRenderer_;
	EndScreenView endScreenView_;
	const GameSystems* gameSystems_ = nullptr;
	int graphicIndex = -1;
	float timeSinceTick_ = 0.0f;
	bool isGameOver_ = false;
};

float GetTimeSinceTick();
}
#endif //SPLASHONLINE_GAME_RENDERER_H_
