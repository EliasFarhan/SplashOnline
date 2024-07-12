//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_GRAPHICS_MANAGER_H_
#define SPLASHONLINE_GRAPHICS_MANAGER_H_

#include "graphics/texture_manager.h"

namespace splash
{

class GraphicsManager
{
public:

	void Begin();
	void Update(float dt);
	void End();
	void Draw();
private:
	TextureManager textureManager_;
};

}

#endif //SPLASHONLINE_GRAPHICS_MANAGER_H_
