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
		template<typename T, nByte typeCode> class CustomType;
		class CustomTypeBase;
		template<typename EKeyType, typename EValueType> class Dictionary;
		class DictionaryBase;
		class Hashtable;
		class JString;
		class Object;

		namespace Helpers
		{
			namespace TypeCode18
			{
				template<bool> struct TypeRange; // intentionally undefined!
				struct Invalid; // intentionally undefined!

				template<nByte C, typename=TypeRange<true> > struct TypeForCode{typedef Invalid type;};
				template<nByte C> struct TypeForCode<C, TypeRange<(C>=CUSTOM_TYPE_SLIM&&C<=CUSTOM_TYPE_SLIM_MAX)> >{typedef CustomTypeBase type;};
				template<> struct TypeForCode<UNKNOWN> {typedef Object type;};
				template<> struct TypeForCode<BOOLEAN> {typedef bool type;};
				template<> struct TypeForCode<BYTE> {typedef nByte type;};
				template<> struct TypeForCode<SHORT> {typedef short type;};
				template<> struct TypeForCode<FLOAT> {typedef float type;};
				template<> struct TypeForCode<DOUBLE> {typedef double type;};
				template<> struct TypeForCode<STRING> {typedef JString type;};
				template<> struct TypeForCode<EG_NULL> {typedef void* type;};
				template<> struct TypeForCode<COMPRESSED_INT> {typedef int type;};
				template<> struct TypeForCode<COMPRESSED_LONG> {typedef int64 type;};
				template<> struct TypeForCode<COMPRESSED_UINT> {typedef unsigned int type;};
				template<> struct TypeForCode<COMPRESSED_ULONG> {typedef uint64 type;};
				template<> struct TypeForCode<INT1> {typedef int type;};
				template<> struct TypeForCode<INT1_> {typedef int type;};
				template<> struct TypeForCode<INT2> {typedef int type;};
				template<> struct TypeForCode<INT2_> {typedef int type;};
				template<> struct TypeForCode<L1> {typedef int64 type;};
				template<> struct TypeForCode<L1_> {typedef int64 type;};
				template<> struct TypeForCode<L2> {typedef int64 type;};
				template<> struct TypeForCode<L2_> {typedef int64 type;};
				template<> struct TypeForCode<CUSTOM> {typedef CustomTypeBase type;};
				template<> struct TypeForCode<DICTIONARY> {typedef DictionaryBase type;};
				template<> struct TypeForCode<HASHTABLE> {typedef Hashtable type;};
				template<> struct TypeForCode<OBJECT_ARRAY> {typedef Object type;};
				template<> struct TypeForCode<BOOLEAN_FALSE> {typedef bool type;};
				template<> struct TypeForCode<BOOLEAN_TRUE> {typedef bool type;};
				template<> struct TypeForCode<SHORT_ZERO> {typedef short type;};
				template<> struct TypeForCode<INT_ZERO> {typedef int type;};
				template<> struct TypeForCode<LONG_ZERO> {typedef int64 type;};
				template<> struct TypeForCode<FLOAT_ZERO> {typedef float type;};
				template<> struct TypeForCode<DOUBLE_ZERO> {typedef double type;};
				template<> struct TypeForCode<BYTE_ZERO> {typedef nByte type;};
				template<> struct TypeForCode<ARRAY> {typedef void* type;};
				template<> struct TypeForCode<BOOLEAN_ARRAY> {typedef bool type;};
				template<> struct TypeForCode<BYTE_ARRAY> {typedef nByte type;};
				template<> struct TypeForCode<SHORT_ARRAY> {typedef short type;};
				template<> struct TypeForCode<FLOAT_ARRAY> {typedef float type;};
				template<> struct TypeForCode<DOUBLE_ARRAY> {typedef double type;};
				template<> struct TypeForCode<STRING_ARRAY> {typedef JString type;};
				template<> struct TypeForCode<COMPRESSED_INT_ARRAY> {typedef int type;};
				template<> struct TypeForCode<COMPRESSED_LONG_ARRAY> {typedef int64 type;};
				template<> struct TypeForCode<CUSTOM_TYPE_ARRAY> {typedef CustomTypeBase type;};
				template<> struct TypeForCode<DICTIONARY_ARRAY> {typedef DictionaryBase type;};
				template<> struct TypeForCode<HASHTABLE_ARRAY> {typedef Hashtable type;};
			}
		}

		/** @file */
	}
}
