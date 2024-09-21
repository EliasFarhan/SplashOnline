//
// Created by unite on 19.08.2024.
//

#include "game/splash.h"
#include "engine/engine.h"
#include "graphics/const.h"

#ifdef ENABLE_DESYNC_DEBUG
#include "utils/game_db.h"
#endif

namespace splash
{

void SplashManager::OnGui()
{

}
void SplashManager::SetGuiIndex(int index)
{
	guiIndex_ = index;
}
int SplashManager::GetGuiIndex() const
{
	return guiIndex_;
}

void SplashManager::Begin()
{

}

void SplashManager::End()
{
	RemoveDrawInterface(this);
	RemoveSystem(this);
	RemoveGuiInterface(this);
}

void SplashManager::Update(float dt)
{
	if(!IsSpineLoaded() || !IsFmodLoaded())
	{
		return;
	}
	if(!logo_ && state_ == State::LOGO)
	{
		logo_ = CreateSkeletonDrawable(SpineManager::KWAKWA_LOGO);
		logo_->animationState->setAnimation(0,"kwakwa",false);
		PlayMusic();
		logoTimer_.Reset();
		bg_ = GetTexture(TextureManager::TextureId::BG);
		controls_ = GetTexture(TextureManager::TextureId::CONTROLS);
	}

	switch(state_)
	{

	case State::LOGO:
	{
		logoTimer_.Update(dt);
		const auto scale = GetGraphicsScale();
		logo_->skeleton->setScaleX(scale);
		logo_->skeleton->setScaleY(scale);

		const auto position = GetGraphicsPosition({});
		logo_->skeleton->setPosition((float)position.x, (float)position.y);
		logo_->update(dt, spine::Physics_Update);
		static bool switchToTitle = false;
		if(logoTimer_.Over())
		{
			this->SwitchToState(State::LOBBY);
		}
		else
		{
			if(logoTimer_.CurrentTime() > 1.5f)
			{
				if(!switchToTitle)
				{
					switchToTitle = true;
					SetMusicParameter("Transition Intro", 0.5f);
				}
				else
				{
					SetMusicParameter("Transition Intro", 0.0f);
					SetMusicParameter("Transition Title", 0.5f);
				}
			}
		}
		break;
	}
	case State::LOBBY:
	{
		if(client_->GetState() == NetworkClient::State::IN_GAME)
		{
			SwitchToState(State::GAME);
		}
		break;
	}
	case State::GAME:
	{
		if(client_->GetState() == NetworkClient::State::IN_ROOM)
		{
			gameManager_->End();
			gameManager_ = nullptr;
			SwitchToState(State::LOBBY);
		}

		break;
	}
	case State::VICTORY_SCREEN:
	{
		break;
	}
	}


}

int SplashManager::GetSystemIndex() const
{
	return systemIndex_;
}

void SplashManager::SetSystemIndex(int index)
{
	systemIndex_ = index;
}

void SplashManager::Draw()
{
	if(!logo_)
	{
		return;
	}
	auto* renderer = GetRenderer();
	switch (state_)
	{

	case State::LOGO:
	{
		logo_->draw(renderer);
		break;
	}
	case State::LOBBY:
	{
		constexpr neko::Vec2f size{neko::Scalar{19.2f}, neko::Scalar{10.8f}};
		auto rect = GetDrawingRect({}, size);
		SDL_RenderCopy(renderer, bg_, nullptr, &rect);
		const neko::Vec2f controlSize{ (neko::Vec2<float>)GetTextureSize(TextureManager::TextureId::CONTROLS)/pixelPerMeter};
		rect = GetDrawingRect({}, controlSize);
		SDL_RenderCopy(renderer, controls_, nullptr, &rect);
		break;
	}
	case State::GAME:
		break;
	case State::VICTORY_SCREEN:
		break;
	}
}

void SplashManager::SetGraphicsIndex(int index)
{
	graphicsIndex_ = index;
}

int SplashManager::GetGraphicsIndex() const
{
	return graphicsIndex_;
}

SplashManager::SplashManager()
{
	AddDrawInterface(this);
	AddGuiInterface(this);
	AddSystem(this);
}
void SplashManager::SwitchToState(SplashManager::State state)
{
	//const auto previousState = state_;
	state_ = state;
	switch (state)
	{
	case State::LOGO:
		break;
	case State::LOBBY:
	{
		state_ = State::LOBBY;
		SetMusicParameter("Transition Intro", 0.0f);
		SetMusicParameter("Transition Kittymanjaro", 0.0f);
		SetMusicParameter("Transition Title", 0.5f);


		if(!client_)
		{
			ExitGames::LoadBalancing::ClientConstructOptions clientConstructOptions{};
			clientConstructOptions.setRegionSelectionMode(ExitGames::LoadBalancing::RegionSelectionMode::SELECT);
			client_ = std::make_unique<NetworkClient>(clientConstructOptions);
			client_->Begin();
		}
		break;
	}
	case State::GAME:
	{
		GameData gameData{};
		gameData.connectedPlayers = client_->GetConnectedPlayers();
		gameManager_ = std::make_unique<GameManager>(gameData);
		gameManager_->Begin();
		SetMusicParameter("Transition Title", 0.0f);
		SetMusicParameter("Transition Kittymanjaro", 0.5f);
		SetMusicParameter("Transition Start", 0.5f);
#ifdef ENABLE_DESYNC_DEBUG
		OpenDatabase(client_->GetPlayerIndex()-1);
#endif
		break;
	}
	case State::VICTORY_SCREEN:
		break;
	}
}
}