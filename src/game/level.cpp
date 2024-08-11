//
// Created by unite on 08.08.2024.
//

#include "game/level.h"

namespace splash
{

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