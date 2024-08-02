//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_RENDERER_H_
#define SPLASHONLINE_GAME_RENDERER_H_

#include "graphics/graphics_manager.h"
#include "game/graphics/player_renderer.h"
#include "game/graphics/bullet_renderer.h"

namespace splash
{

class GameRenderer : public DrawInterface
{
public:
	void Begin();
	void Update(float dt);
	void End();
	void Draw() override;
	void SetIndex(int index) override;
	int GetIndex() const override;
public:

private:
	PlayerRenderer playerRenderer_;
	BulletRenderer bulletRenderer_;
	int graphicIndex = -1;
};

}
#endif //SPLASHONLINE_GAME_RENDERER_H_
