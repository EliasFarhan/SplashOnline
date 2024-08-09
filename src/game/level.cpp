//
// Created by unite on 08.08.2024.
//

#include "game/level.h"

namespace splash
{

static constexpr std::array<Platform, 3> platforms
	{
	{
		{neko::Vec2f{neko::Fixed16 {0}, neko::Fixed16 {0}},
		 neko::Vec2f{neko::Fixed16{-0.04f}, neko::Fixed16{-0.05741876f}},
		 neko::Vec2f {neko::Fixed16 {4.973756f}, neko::Fixed16 {0.7651642f}}},

		{neko::Vec2f{neko::Fixed16 {-5.1f}, neko::Fixed16 {-2.62f}},
		 neko::Vec2f{neko::Fixed16{0}, neko::Fixed16{-0.02366066f}},
		 neko::Vec2f {neko::Fixed16 {3.62f}, neko::Fixed16 {1.684614f}}},

		{neko::Vec2f{neko::Fixed16 {5.1f}, neko::Fixed16 {-2.59f}},
		 neko::Vec2f{neko::Fixed16{-0.02076483f}, neko::Fixed16{-0.01601195f}},
		 neko::Vec2f {neko::Fixed16 {3.52847f}, neko::Fixed16 {1.428922f}}},
	}
	};

neko::Span<Platform> GetPlatforms()
{
	return {platforms};
}
void Level::Begin()
{
	int i = 0;
	for(const auto& platform : platforms)
	{
		auto& pair = platformPhysics_[i];
		pair.first = world_->AddBody();
		pair.second = world_->AddAabbCollider(pair.first);

		auto& body = world_->body(pair.first);
		body.position = platform.position;
		body.type = neko::BodyType::STATIC;

		auto& collider = world_->collider(pair.second);
		collider.offset = platform.offset;
		auto& aabb = world_->aabb(collider.shapeIndex);
		aabb.halfSize = platform.size/neko::Scalar{2};
		i++;
	}
}
void Level::End()
{

}
Level::Level(neko::PhysicsWorld* world): world_(world)
{

}
}