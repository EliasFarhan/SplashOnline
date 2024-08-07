//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_SPINE_MANAGER_H_
#define SPLASHONLINE_SPINE_MANAGER_H_

#include "spine/TextureLoader.h"

namespace splash
{

class SpineManager : public spine::TextureLoader
{
public:
	enum class AtlasId
	{
		KWAKWA_LOGO,
		ANNOUNCERS,
		CHARACTERS_NOARM,
		CHARACTERS_ARMS,
		CLOUD,
		BASEGUN,
		FX,
		LENGTH,
	};
	void load(spine::AtlasPage& page, const spine::String& path) override;

	void unload(void* texture) override;


};

}
#endif //SPLASHONLINE_SPINE_MANAGER_H_
