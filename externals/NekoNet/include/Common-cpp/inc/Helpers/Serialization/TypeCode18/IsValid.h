/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/TypeForCode.h"
#include "Common-cpp/inc/Helpers/TypeTraits/IsSame.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace TypeCode18
			{
				template<nByte C> struct IsValid{static const bool is = !(IsSame<typename TypeForCode<C>::type, Invalid>::is || C==COMPRESSED_UINT || C==COMPRESSED_ULONG);};
			}
		}

		/** @file */
	}
}