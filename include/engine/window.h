//
// Created by unite on 14.07.2024.
//

#ifndef SPLASHONLINE_WINDOW_H_
#define SPLASHONLINE_WINDOW_H_

#include <SDL.h>
#include <vector>

namespace splash
{
class OnEventInterface
{
public:
	virtual void OnEvent(const SDL_Event& event) = 0;
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
	[[nodiscard]] bool IsOpen() const noexcept;
    [[nodiscard]] std::pair<int, int> GetWindowSize() const;
private:
    std::vector<OnEventInterface*> eventInterfaces_;
	SDL_Window* window_ = nullptr;
    bool isOpen_ = false;
};

SDL_Window* GetWindow();
void AddEventListener(OnEventInterface* eventInterface);
std::pair<int, int> GetWindowSize();
}
#endif //SPLASHONLINE_WINDOW_H_
