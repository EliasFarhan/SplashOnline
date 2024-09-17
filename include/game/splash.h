//
// Created by unite on 19.08.2024.
//

#ifndef SPLASHONLINE_ENGINE_SPLASH_H_
#define SPLASHONLINE_ENGINE_SPLASH_H_

#include <memory>
#include "game/game_manager.h"
#include "network/client.h"
#include "audio/music_manager.h"

namespace splash
{

class SplashManager : public OnGuiInterface,
	public SystemInterface,
	public DrawInterface
{
public:
	enum class State
	{
		LOGO,
		LOBBY,
		GAME,
		VICTORY_SCREEN
	};
	SplashManager();

	void OnGui() override;
	void SetGuiIndex(int index) override;
	int GetGuiIndex() const override;

	void Begin() override;

	void End() override;

	void Update(float dt) override;

	int GetSystemIndex() const override;

	void SetSystemIndex(int index) override;

	void Draw() override;

	void SetGraphicsIndex(int index) override;

	int GetGraphicsIndex() const override;

private:
	std::unique_ptr<NetworkClient> client_{};
	std::unique_ptr<GameManager> gameManager_{};

	Timer<float, 2.0f> logoTimer_{-1.0f};

	std::unique_ptr<spine::SkeletonDrawable> logo_ = nullptr;
	SDL_Texture* bg_ = nullptr;
	SDL_Texture* controls_ = nullptr;
	State state_ = State::LOGO;
	int systemIndex_ = -1;
	int graphicsIndex_ = -1;
	int guiIndex_ = -1;

	void SwitchToState(State state);
};

}

#endif //SPLASHONLINE_INCLUDE_ENGINE_SPLASH_H_
