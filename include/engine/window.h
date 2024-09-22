//
// Created by unite on 14.07.2024.
//

#ifndef SPLASHONLINE_WINDOW_H_
#define SPLASHONLINE_WINDOW_H_
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
#include <SDL.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include <vector>

namespace splash
{
class OnEventInterface
{
public:
	virtual ~OnEventInterface() = default;
	virtual void OnEvent(const SDL_Event& event) = 0;
	[[nodiscard]]virtual int GetEventListenerIndex() const = 0;
	virtual void SetEventListenerIndex(int index) = 0;
};

class Window
{
public:
	Window();
	void Begin();
    void Update();
	void End();
	SDL_Window* GetWindow();
    void AddEventListener(OnEventInterface* eventInterface);
    void RemoveEventListener(OnEventInterface* eventInterface);
	[[nodiscard]] bool IsOpen() const noexcept;
    [[nodiscard]] std::pair<int, int> GetWindowSize() const;
private:
    std::vector<OnEventInterface*> eventInterfaces_;
	SDL_Window* window_ = nullptr;
    bool isOpen_ = false;
};

SDL_Window* GetWindow();
void AddEventListener(OnEventInterface* eventInterface);
void RemoveEventListener(OnEventInterface* eventInterface);
std::pair<int, int> GetWindowSize();
}
#endif //SPLASHONLINE_WINDOW_H_
