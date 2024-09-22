//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_BULLET_RENDERER_H_
#define SPLASHONLINE_BULLET_RENDERER_H_

#include "game/bullet.h"

#include <container/vector.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#include <spine-sdl-cpp.h>
#pragma clang diagnostic pop

#include <memory>

namespace splash
{
struct BulletRenderData
{
	enum class BulletRenderState
	{
		NONE,
		WATA,
		DESTROYED
	};
	std::unique_ptr<spine::SkeletonDrawable> drawable{};
	BulletRenderState state = BulletRenderState::NONE;

};
class BulletView
{
public:
	explicit BulletView(const GameSystems* gameSystems);
	void Begin();

	void End();

	void Update(float dt);

	void Draw();

private:
	void Load();
	const GameSystems* gameSystems_ = nullptr;
	std::array<BulletRenderData, BulletManager::MaxBulletNmb> bulletRenderDatas_{};
};
}
#endif //SPLASHONLINE_BULLET_RENDERER_H_
