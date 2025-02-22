/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Serialization/TypeCode/IsValid.h"

namespace ExitGames
{
	namespace Common
	{
		namespace TypeCode
		{
			template<nByte C> struct IsValid1DArrayElement{static const bool is = IsValid<C>::is&&C!=TypeCode::UNKNOWN&&C!=TypeCode::ARRAY&&C!=TypeCode::EG_NULL;};
		}
		/** @file */
	}
}