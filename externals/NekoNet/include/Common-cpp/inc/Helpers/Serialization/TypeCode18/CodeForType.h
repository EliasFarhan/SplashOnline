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
				template<typename T, unsigned int N=0, typename=void> struct CodeForType {};
				template<> struct CodeForType<Object>{static const nByte typeCode = UNKNOWN;};
				template<> struct CodeForType<bool>{static const nByte typeCode = BOOLEAN;};
				template<> struct CodeForType<nByte>{static const nByte typeCode = BYTE;};
				template<> struct CodeForType<short>{static const nByte typeCode = SHORT;};
				template<> struct CodeForType<float>{static const nByte typeCode = FLOAT;};
				template<> struct CodeForType<double>{static const nByte typeCode = DOUBLE;};
				template<> struct CodeForType<JString>{static const nByte typeCode = STRING;};
				template<> struct CodeForType<char*>{static const nByte typeCode = STRING;};
				template<> struct CodeForType<const char*>{static const nByte typeCode = STRING;};
				template<size_t N> struct CodeForType<char[N]>{static const nByte typeCode = STRING;};
				template<size_t N> struct CodeForType<const char[N]>{static const nByte typeCode = STRING;};
				template<> struct CodeForType<wchar_t*>{static const nByte typeCode = STRING;};
				template<> struct CodeForType<const wchar_t*>{static const nByte typeCode = STRING;};
				template<size_t N> struct CodeForType<wchar_t[N]>{static const nByte typeCode = STRING;};
				template<size_t N> struct CodeForType<const wchar_t[N]>{static const nByte typeCode = STRING;};
				template<> struct CodeForType<int>{static const nByte typeCode = COMPRESSED_INT;};
				template<> struct CodeForType<int64>{static const nByte typeCode = COMPRESSED_LONG;};
				template<> struct CodeForType<unsigned int>{static const nByte typeCode = COMPRESSED_UINT;};
				template<> struct CodeForType<uint64>{static const nByte typeCode = COMPRESSED_ULONG;};
				template<typename C, nByte customCode> struct CodeForType<CustomType<C, customCode> >{static const nByte typeCode = CUSTOM;};
				template<> struct CodeForType<CustomTypeBase>{static const nByte typeCode = CUSTOM;};
				template<typename T> struct CodeForType<T, 0, typename EnableIf<IsDerivedFrom<T, DictionaryBase>::is>::type>{static const nByte typeCode = DICTIONARY;};
				template<> struct CodeForType<Hashtable>{static const nByte typeCode = HASHTABLE;};
				template<> struct CodeForType<Object*>{static const nByte typeCode = OBJECT_ARRAY;};
				template<> struct CodeForType<bool*>{static const nByte typeCode = BOOLEAN_ARRAY;};
				template<> struct CodeForType<nByte*>{static const nByte typeCode = BYTE_ARRAY;};
				template<> struct CodeForType<short*>{static const nByte typeCode = SHORT_ARRAY;};
				template<> struct CodeForType<float*>{static const nByte typeCode = FLOAT_ARRAY;};
				template<> struct CodeForType<double*>{static const nByte typeCode = DOUBLE_ARRAY;};
				template<> struct CodeForType<JString*>{static const nByte typeCode = STRING_ARRAY;};
				template<> struct CodeForType<char**>{static const nByte typeCode = STRING_ARRAY;};
				template<size_t N> struct CodeForType<char*[N]>{static const nByte typeCode = STRING_ARRAY;};
				template<size_t N, size_t O> struct CodeForType<char[N][O]>{static const nByte typeCode = STRING_ARRAY;};
				template<> struct CodeForType<wchar_t**>{static const nByte typeCode = STRING_ARRAY;};
				template<size_t N> struct CodeForType<wchar_t*[N]>{static const nByte typeCode = STRING_ARRAY;};
				template<size_t N, size_t O> struct CodeForType<wchar_t[N][O]>{static const nByte typeCode = STRING_ARRAY;};
				template<> struct CodeForType<int*>{static const nByte typeCode = COMPRESSED_INT_ARRAY;};
				template<> struct CodeForType<int64*>{static const nByte typeCode = COMPRESSED_LONG_ARRAY;};
				template<typename C, nByte customCode> struct CodeForType<CustomType<C, customCode>*>{static const nByte typeCode = CUSTOM_TYPE_ARRAY;};
				template<typename T> struct CodeForType<T*, 0, typename EnableIf<IsDerivedFrom<T, DictionaryBase>::is>::type>{static const nByte typeCode = DICTIONARY_ARRAY;};
				template<> struct CodeForType<Hashtable*>{static const nByte typeCode = HASHTABLE_ARRAY;};
				template<typename T> struct CodeForType<T*>{static const nByte typeCode = ARRAY;};
				template<typename T> struct CodeForType<const T>{static const nByte typeCode = CodeForType<T>::typeCode;};
				template<typename T> struct CodeForType<const T*>{static const nByte typeCode = CodeForType<T*>::typeCode;};
				template<typename T> struct CodeForType<const T**>{static const nByte typeCode = CodeForType<T**>::typeCode;};
				template<typename T, size_t N> struct CodeForType<T[N]>{static const nByte typeCode = CodeForType<T*>::typeCode;};
			}
		}

		/** @file */
	}
}