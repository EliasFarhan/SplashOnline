//
// Created by unite on 02.08.2024.
//

#include "game/graphics/bullet_view.h"
#include "graphics/graphics_manager.h"
#include "game/game_systems.h"
#include "audio/audio_manager.h"
#include "audio/player_sound.h"
#include "container/vector.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

void BulletView::Begin()
{

}

void BulletView::End()
{

}

void BulletView::Update(float dt)
{
	if (!IsSpineLoaded())
		return;
	if(bulletRenderDatas_[0].drawable == nullptr)
	{
		Load();
	}
	const auto& bulletManager = gameSystems_->GetBulletManager();
	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();

	const auto scale = (float)Bullet::Scale * GetGraphicsScale();
	for(int i = 0; i < BulletManager::MaxBulletNmb; i++)
	{
		const auto& bullet = bulletManager.GetBullets()[i];
		auto& bulletRenderData = bulletRenderDatas_[i];
		const auto& body = physicsWorld.body(bullet.bodyIndex);
		switch(bulletRenderData.state)
		{

		case BulletRenderData::BulletRenderState::NONE:
		{
			if(bullet.playerNumber != -1 && body.isActive)
			{
				if(body.type == neko::BodyType::KINEMATIC)
				{
					FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::DOUBLEGUN));
				}
				else
				{
					FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::GUN));
				}
				bulletRenderData.state = BulletRenderData::BulletRenderState::WATA;
				bulletRenderData.drawable->animationState->setAnimation(0, "water_fly", true);
				bulletRenderData.previousPositions.clear();
				bulletRenderData.previousPositions.insert(bulletRenderData.previousPositions.cbegin(), body.position);
				break;
			}
			if(bullet.playerNumber != -1 && !body.isActive && !bullet.destroyedTimer.Over())
			{
				bulletRenderData.state = BulletRenderData::BulletRenderState::DESTROYED;
				bulletRenderData.drawable->animationState->setAnimation(0, "water_destroy", false);
				break;
			}
			continue;
		}
		case BulletRenderData::BulletRenderState::WATA:
		{
			if(bullet.playerNumber == -1)
			{
				bulletRenderData.state = BulletRenderData::BulletRenderState::NONE;
				continue;
			}
			if(!bullet.destroyedTimer.Over())
			{
				FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::IMPACT1));
				bulletRenderData.state = BulletRenderData::BulletRenderState::DESTROYED;
				bulletRenderData.drawable->animationState->setAnimation(0, "water_destroy", false);
			}
			else
			{
				if(bulletRenderData.previousPositions[0] != body.position)
				{
					if(bulletRenderData.previousPositions.is_full())
					{
						bulletRenderData.previousPositions.erase(bulletRenderData.previousPositions.cend()-1);
					}
					bulletRenderData.previousPositions.insert(bulletRenderData.previousPositions.cbegin(), body.position);
				}

			}


			break;
		}
		case BulletRenderData::BulletRenderState::DESTROYED:
		{
			if(bullet.destroyedTimer.Over())
			{
				bulletRenderData.state = BulletRenderData::BulletRenderState::NONE;
				continue;
			}

			break;
		}
		}
		bulletRenderData.drawable->skeleton->setScaleX(scale);
		bulletRenderData.drawable->skeleton->setScaleY(scale);
		const auto position = GetGraphicsPosition(body.position);
		bulletRenderData.drawable->skeleton->setPosition((float)position.x, (float)position.y);

		bulletRenderData.drawable->update(dt, spine::Physics_Update);

	}
}

void BulletView::Draw()
{
	auto* renderer = GetRenderer();
	const auto& bulletManager = gameSystems_->GetBulletManager();

	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	for(int i = 0; i < BulletManager::MaxBulletNmb; i++)
	{
		const auto& bullet = bulletManager.GetBullets()[i];
		auto& bulletRenderData = bulletRenderDatas_[i];
		if(bulletRenderData.state == BulletRenderData::BulletRenderState::NONE)
		{
			continue;
		}


		if(bulletRenderData.previousPositions.size() > 1 && bulletRenderData.state == BulletRenderData::BulletRenderState::WATA)
		{
			int positionsCount = (int)bulletRenderData.previousPositions.size();
			std::array<neko::Vec2i, 10> bulletScreenPos{};
			for (int j = 0; j < positionsCount; j++)
			{
				bulletScreenPos[j] = GetGraphicsPosition(bulletRenderData.previousPositions[j]);
			}
			neko::SmallVector<SDL_Vertex, 20> bulletVertices{};
			for (int j = 0; j < positionsCount; j++)
			{
				neko::Vec2i delta;
				if (j == bulletRenderData.previousPositions.size() - 1)
				{
					delta = bulletScreenPos[j] - bulletScreenPos[j - 1];
				}
				else
				{
					delta = bulletScreenPos[j + 1] - bulletScreenPos[j];
				}
				auto dir = neko::Vec2<float>{ delta.Perpendicular() }.Normalized();
				static constexpr float width = 8.0f;
				auto p1 = neko::Vec2<float>{ bulletScreenPos[j] } + dir * (width/(float)(j+1));
				auto p2 = neko::Vec2<float>{ bulletScreenPos[j] } - dir * (width/(float)(j+1));
				SDL_Vertex v{};
				v.position = { p1.x, p1.y };
				v.color = playerColors[bullet.playerNumber];
				v.color.a /= (Uint8)(j+1);
				bulletVertices.push_back(v);
				v.position = { p2.x, p2.y };
				bulletVertices.push_back(v);
			}
			neko::SmallVector<int, 18 * 3> indices{};
			for(int j = 0; j < positionsCount-1; j++)
			{
				indices.push_back(j*2);
				indices.push_back(j*2+1);
				indices.push_back(j*2+3);

				indices.push_back(j*2);
				indices.push_back(j*2+2);
				indices.push_back(j*2+3);
			}
			SDL_BlendMode blendMode;
			SDL_GetRenderDrawBlendMode(renderer, &blendMode);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_RenderGeometry(renderer,
				nullptr,
				bulletVertices.data(),
				(int)bulletVertices.size(),
				indices.data(),
				(int)indices.size());
			SDL_SetRenderDrawBlendMode(renderer, blendMode);
		}
		bulletRenderData.drawable->draw(renderer);
		if (GetDebugConfig().showPhysicsBox)
		{
			const auto& body = physicsWorld.body(bullet.bodyIndex);
			const auto rect = GetDrawingRect(
				body.position,
				neko::Vec2f(Bullet::Radius + Bullet::Radius, Bullet::Radius + Bullet::Radius));
			const auto& color = playerColors[bullet.playerNumber];
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
}

void BulletView::Load()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for(int i = 0; i < BulletManager::MaxBulletNmb; i++)
	{
		bulletRenderDatas_[i].drawable = CreateSkeletonDrawable(SpineManager::WATA);
		bulletRenderDatas_[i].drawable->animationState->setAnimation(0, "water_fly", true);
	}

}

BulletView::BulletView(const GameSystems* gameSystems): gameSystems_(gameSystems)
{

}
}
