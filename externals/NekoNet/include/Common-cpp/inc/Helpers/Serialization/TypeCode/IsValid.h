/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Serialization/TypeCode/TypeForCode.h"
#include "Common-cpp/inc/Helpers/TypeTraits/IsSame.h"

namespace ExitGames
{
	namespace Common
	{
		namespace TypeCode
		{
			template<nByte C> struct IsValid{static const bool is = !Helpers::IsSame<typename TypeForCode<C>::type, Invalid>::is;};
		}
		/** @file */
	}
}