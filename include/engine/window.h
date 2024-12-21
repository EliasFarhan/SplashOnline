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

SDL_Window* GetWindow();
void BeginWindow();
void UpdateWindow();
void EndWindow();
std::pair<int, int> GetWindowSize();
bool IsWindowOpen();

void AddEventListener(OnEventInterface* eventInterface);
void RemoveEventListener(OnEventInterface* eventInterface);

}
#endif //SPLASHONLINE_WINDOW_H_
