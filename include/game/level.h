//
// Created by unite on 08.08.2024.
//

#ifndef SPLASHONLINE_GAME_LEVEL_H_
#define SPLASHONLINE_GAME_LEVEL_H_

#include "game/const.h"

#include <math/vec2.h>
#include <container/span.h>
#include <physics/physics_type.h>
#include <physics/physics.h>

#include <array>
namespace splash
{

class Level
{
public:
	struct Platform
	{
		neko::BodyIndex bodyIndex = neko::INVALID_BODY_INDEX;
		neko::ColliderIndex colliderIndex = neko::INVALID_COLLIDER_INDEX;
		ColliderUserData userData{};
	};
	explicit Level(neko::PhysicsWorld* world);
	void Begin();
	void End();

	static constexpr std::array<Box, 3> platforms
		{
			{
				{neko::Vec2f{neko::Scalar {0}, neko::Scalar {0}},
				 neko::Vec2f{neko::Scalar{-0.04f}, neko::Scalar{-0.05741876f}},
				 neko::Vec2f {neko::Scalar {4.973756f}, neko::Scalar {0.7651642f}}},

				{neko::Vec2f{neko::Scalar {-5.1f}, neko::Scalar {-2.62f}},
				 neko::Vec2f{neko::Scalar{0}, neko::Scalar{-0.02366066f}},
				 neko::Vec2f {neko::Scalar {3.62f}, neko::Scalar {1.684614f}}},

				{neko::Vec2f{neko::Scalar {5.1f}, neko::Scalar {-2.59f}},
				 neko::Vec2f{neko::Scalar{-0.02076483f}, neko::Scalar{-0.01601195f}},
				 neko::Vec2f {neko::Scalar {3.52847f}, neko::Scalar {1.428922f}}},
			}
		};

	static constexpr std::array<std::pair<Box, ColliderUserData>, 4> gameLimits
		{{
			 {{{neko::Scalar {-15}, {}},{neko::Scalar{-5},{}}, {neko::Scalar{10}, neko::Scalar{40} }}, {ColliderType::GAME_LIMIT, -1}},
			 {{{neko::Scalar {15}, {}},{neko::Scalar{5},{}}, {neko::Scalar{10}, neko::Scalar{40} }}, {ColliderType::GAME_LIMIT, -1}},
			 {{{{}, neko::Scalar {-15}},{{},neko::Scalar {-5}}, {neko::Scalar{40}, neko::Scalar{10} }}, {ColliderType::GAME_LIMIT, -1}},
			 {{{{}, neko::Scalar {15}},{{},neko::Scalar {5}}, {neko::Scalar{40}, neko::Scalar{10} }}, {ColliderType::GAME_LIMIT, -1}},
		 }};

private:
	neko::PhysicsWorld* world_;
	std::array<Platform, 3> platformPhysics_;

};



}

#endif //SPLASHONLINE_GAME_LEVEL_H_
