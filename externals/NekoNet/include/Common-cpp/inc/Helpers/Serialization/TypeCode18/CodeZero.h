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
				template<typename T> struct CodeZero{};
				template<> struct CodeZero<short>{static const nByte typeCode = SHORT_ZERO;};
				template<> struct CodeZero<int>{static const nByte typeCode = INT_ZERO;};
				template<> struct CodeZero<int64>{static const nByte typeCode= LONG_ZERO;};
				template<> struct CodeZero<float>{static const nByte typeCode = FLOAT_ZERO;};
				template<> struct CodeZero<double>{static const nByte typeCode = DOUBLE_ZERO;};
				template<> struct CodeZero<nByte>{static const nByte typeCode = BYTE_ZERO;};
			}
		}

		/** @file */
	}
}