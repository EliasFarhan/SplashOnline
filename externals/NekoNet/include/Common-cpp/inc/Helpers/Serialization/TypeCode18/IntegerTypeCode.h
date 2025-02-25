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
				template<typename T> struct IntegerTypeCode
				{
				};

				template<> struct IntegerTypeCode<int>
				{
					static const nByte COMPRESSED        = COMPRESSED_INT;
					static const nByte ONE_BYTE          = INT1;
					static const nByte ONE_BYTE_         = INT1_;
					static const nByte TWO_BYTE          = INT2;
					static const nByte TWO_BYTE_         = INT2_;
					static const nByte ZERO              = INT_ZERO;
				};

				template<> struct IntegerTypeCode<int64>
				{
					static const nByte COMPRESSED        = COMPRESSED_LONG;
					static const nByte ONE_BYTE          = L1;
					static const nByte ONE_BYTE_         = L1_;
					static const nByte TWO_BYTE          = L2;
					static const nByte TWO_BYTE_         = L2_;
					static const nByte ZERO              = LONG_ZERO;
				};
			}
		}

		/** @file */
	}
}