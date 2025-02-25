/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/TypeCode18.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace TypeCode18
			{
				template<nByte C> struct IsCustom{static const bool is = C==CUSTOM||(C>=CUSTOM_TYPE_SLIM&&C<=CUSTOM_TYPE_SLIM_MAX);};
			}
		}

		/** @file */
	}
}