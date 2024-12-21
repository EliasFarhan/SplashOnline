//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_GUI_RENDERER_H_
#define SPLASHONLINE_GUI_RENDERER_H_

#include "engine/window.h"

#include <container/vector.h>

#include <SDL.h>
#include <vector>

namespace splash {

class OnGuiInterface
        {
public:
    virtual ~OnGuiInterface() = default;

    virtual void OnGui() = 0;

    virtual void SetGuiIndex(int index) = 0;

    [[nodiscard]] virtual int GetGuiIndex() const = 0;
};

struct DebugConfig
        {
    bool showPhysicsBox = false;
};

void BeginGuiRenderer();

void UpdateGuiRenderer();

void EndGuiRenderer();

void DrawGuiRenderer();

void ManageGuiEvent(const SDL_Event &event);

void AddGuiInterface(OnGuiInterface *guiInterface);

void RemoveGuiInterface(OnGuiInterface *guiInterface);

const DebugConfig &GetDebugConfig();
}

#endif //SPLASHONLINE_GUI_RENDERER_H_
