//
// Created by unite on 08.08.2024.
//

#ifndef SPLASHONLINE_GAME_LEVEL_H_
#define SPLASHONLINE_GAME_LEVEL_H_

#include <math/vec2.h>
#include <container/span.h>
#include <physics/physics_type.h>
#include <physics/physics.h>

#include <array>
namespace splash
{

struct Platform
{
	neko::Vec2f position{};
	neko::Vec2f offset{};
	neko::Vec2f size{};
};

class Level
{
public:
	explicit Level(neko::PhysicsWorld* world);
	void Begin();
	void End();
private:
	neko::PhysicsWorld* world_;
	std::array<std::pair<neko::BodyIndex, neko::ColliderIndex>, 3> platformPhysics_;
};

neko::Span<Platform> GetPlatforms();

}

#endif //SPLASHONLINE_GAME_LEVEL_H_
