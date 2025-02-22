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
				template<nByte C> struct IsArray{static const bool is = false;};
				template<> struct IsArray<ARRAY>{static const bool is = true;};
				template<> struct IsArray<OBJECT_ARRAY>{static const bool is = true;};
				template<> struct IsArray<BOOLEAN_ARRAY>{static const bool is = true;};
				template<> struct IsArray<BYTE_ARRAY>{static const bool is = true;};
				template<> struct IsArray<SHORT_ARRAY>{static const bool is = true;};
				template<> struct IsArray<FLOAT_ARRAY>{static const bool is = true;};
				template<> struct IsArray<DOUBLE_ARRAY>{static const bool is = true;};
				template<> struct IsArray<STRING_ARRAY>{static const bool is = true;};
				template<> struct IsArray<COMPRESSED_INT_ARRAY>{static const bool is = true;};
				template<> struct IsArray<COMPRESSED_LONG_ARRAY>{static const bool is = true;};
				template<> struct IsArray<CUSTOM_TYPE_ARRAY>{static const bool is = true;};
				template<> struct IsArray<DICTIONARY_ARRAY>{static const bool is = true;};
				template<> struct IsArray<HASHTABLE_ARRAY>{static const bool is = true;};
			}
		}

		/** @file */
	}
}