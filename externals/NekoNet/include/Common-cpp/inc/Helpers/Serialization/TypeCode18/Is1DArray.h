/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/IsArray.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace TypeCode18
			{
				template<nByte C> struct Is1DArray{static const bool is = IsArray<C>::is && C!=ARRAY;};
			}
		}

		/** @file */
	}
}