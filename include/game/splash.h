//
// Created by unite on 19.08.2024.
//

#ifndef SPLASHONLINE_ENGINE_SPLASH_H_
#define SPLASHONLINE_ENGINE_SPLASH_H_

#include <memory>
#include "game/game_manager.h"
#include "network/client.h"

namespace splash
{

class SplashManager : public OnGuiInterface
{
public:
	void OnGui() override;
	void SetGuiIndex(int index) override;
	int GetGuiIndex() const override;
public:
private:
	NetworkClient client_{};
	std::unique_ptr<GameManager> gameManager_{};

};

}

#endif //SPLASHONLINE_INCLUDE_ENGINE_SPLASH_H_
