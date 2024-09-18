//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_GUI_RENDERER_H_
#define SPLASHONLINE_GUI_RENDERER_H_

#include "engine/window.h"

#include <container/vector.h>

#include <SDL.h>
#include <vector>

namespace splash
{

class OnGuiInterface
{
public:
	virtual void OnGui() = 0;
	virtual void SetGuiIndex(int index) = 0;
	[[nodiscard]] virtual int GetGuiIndex() const = 0;
};

struct DebugConfig
{
	bool showPhysicsBox = false;
};

class GuiRenderer : public OnEventInterface
{
public:
	GuiRenderer();
	void Begin();
	void Update();
	void End();

	void Draw();

	void AddGuiInterface(OnGuiInterface* guiInterface);
	void RemoveGuiInterface(OnGuiInterface* guiInterface);
	void OnEvent(const SDL_Event& event) override;

	int GetEventListenerIndex() const override;

	void SetEventListenerIndex(int index) override;

	[[nodiscard]] const auto& GetDebugConfig() { return debugConfig_; }
private:
	SDL_Renderer* renderer_;
	std::vector<OnGuiInterface*> guiInterfaces_;
	DebugConfig debugConfig_{};
	int eventListenerIndex_ = -1;
	neko::SmallVector<float, 20> deltaTimes_{};

};

void AddGuiInterface(OnGuiInterface* guiInterface);
void RemoveGuiInterface(OnGuiInterface* guiInterface);

const DebugConfig& GetDebugConfig();
}

#endif //SPLASHONLINE_GUI_RENDERER_H_
