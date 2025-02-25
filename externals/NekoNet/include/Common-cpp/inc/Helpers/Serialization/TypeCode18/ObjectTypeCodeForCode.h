/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Enums/TypeCode.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/TypeCode18.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace TypeCode18
			{
				template<bool> struct TypeRange; // intentionally undefined!

				template<nByte C, typename=TypeRange<true> > struct ObjectTypeCodeForCode{static const nByte code = TypeCode::INVALID;};
				template<nByte C> struct ObjectTypeCodeForCode<C, TypeRange<(C>=CUSTOM_TYPE_SLIM&&C<=CUSTOM_TYPE_SLIM_MAX)> >{static const nByte code = TypeCode::CUSTOM;};
				template<> struct ObjectTypeCodeForCode<UNKNOWN> {static const nByte code = TypeCode::OBJECT;};
				template<> struct ObjectTypeCodeForCode<BOOLEAN> {static const nByte code = TypeCode::BOOLEAN;};
				template<> struct ObjectTypeCodeForCode<BYTE> {static const nByte code = TypeCode::BYTE;};
				template<> struct ObjectTypeCodeForCode<SHORT> {static const nByte code = TypeCode::SHORT;};
				template<> struct ObjectTypeCodeForCode<FLOAT> {static const nByte code = TypeCode::FLOAT;};
				template<> struct ObjectTypeCodeForCode<DOUBLE> {static const nByte code = TypeCode::DOUBLE;};
				template<> struct ObjectTypeCodeForCode<STRING> {static const nByte code = TypeCode::STRING;};
				template<> struct ObjectTypeCodeForCode<COMPRESSED_INT> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<COMPRESSED_LONG> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<INT1> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<INT1_> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<INT2> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<INT2_> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<L1> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<L1_> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<L2> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<L2_> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<EG_NULL> {static const nByte code = TypeCode::EG_NULL;};
				template<> struct ObjectTypeCodeForCode<CUSTOM> {static const nByte code = TypeCode::CUSTOM;};
				template<> struct ObjectTypeCodeForCode<DICTIONARY> {static const nByte code = TypeCode::DICTIONARY;};
				template<> struct ObjectTypeCodeForCode<HASHTABLE> {static const nByte code = TypeCode::HASHTABLE;};
				template<> struct ObjectTypeCodeForCode<OBJECT_ARRAY> {static const nByte code = TypeCode::OBJECT;};
				template<> struct ObjectTypeCodeForCode<BOOLEAN_FALSE> {static const nByte code = TypeCode::BOOLEAN;};
				template<> struct ObjectTypeCodeForCode<BOOLEAN_TRUE> {static const nByte code = TypeCode::BOOLEAN;};
				template<> struct ObjectTypeCodeForCode<SHORT_ZERO> {static const nByte code = TypeCode::SHORT;};
				template<> struct ObjectTypeCodeForCode<INT_ZERO> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<LONG_ZERO> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<FLOAT_ZERO> {static const nByte code = TypeCode::FLOAT;};
				template<> struct ObjectTypeCodeForCode<DOUBLE_ZERO> {static const nByte code = TypeCode::DOUBLE;};
				template<> struct ObjectTypeCodeForCode<BYTE_ZERO> {static const nByte code = TypeCode::BYTE;};
				template<> struct ObjectTypeCodeForCode<BOOLEAN_ARRAY> {static const nByte code = TypeCode::BOOLEAN;};
				template<> struct ObjectTypeCodeForCode<BYTE_ARRAY> {static const nByte code = TypeCode::BYTE;};
				template<> struct ObjectTypeCodeForCode<SHORT_ARRAY> {static const nByte code = TypeCode::SHORT;};
				template<> struct ObjectTypeCodeForCode<FLOAT_ARRAY> {static const nByte code = TypeCode::FLOAT;};
				template<> struct ObjectTypeCodeForCode<DOUBLE_ARRAY> {static const nByte code = TypeCode::DOUBLE;};
				template<> struct ObjectTypeCodeForCode<STRING_ARRAY> {static const nByte code = TypeCode::STRING;};
				template<> struct ObjectTypeCodeForCode<COMPRESSED_INT_ARRAY> {static const nByte code = TypeCode::INTEGER;};
				template<> struct ObjectTypeCodeForCode<COMPRESSED_LONG_ARRAY> {static const nByte code = TypeCode::LONG;};
				template<> struct ObjectTypeCodeForCode<CUSTOM_TYPE_ARRAY> {static const nByte code = TypeCode::CUSTOM;};
				template<> struct ObjectTypeCodeForCode<DICTIONARY_ARRAY> {static const nByte code = TypeCode::DICTIONARY;};
				template<> struct ObjectTypeCodeForCode<HASHTABLE_ARRAY> {static const nByte code = TypeCode::HASHTABLE;};
			}
		}

		/** @file */
	}
}