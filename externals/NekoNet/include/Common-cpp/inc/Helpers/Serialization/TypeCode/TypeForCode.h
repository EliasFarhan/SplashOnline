/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Enums/TypeCode.h"

namespace ExitGames
{
	namespace Common
	{
		class CustomTypeBase;
		class DictionaryBase;
		class Hashtable;
		class JString;
		class Object;

		namespace TypeCode
		{
			struct Invalid; // intentionally undefined!

			template<nByte C> struct TypeForCode{typedef Invalid type;};
			template<> struct TypeForCode<BYTE> {typedef nByte type;};
			template<> struct TypeForCode<SHORT> {typedef short type;};
			template<> struct TypeForCode<INTEGER> {typedef int type;};
			template<> struct TypeForCode<LONG> {typedef int64 type;};
			template<> struct TypeForCode<FLOAT> {typedef float type;};
			template<> struct TypeForCode<DOUBLE> {typedef double type;};
			template<> struct TypeForCode<BOOLEAN> {typedef bool type;};
			template<> struct TypeForCode<EG_NULL> {typedef void* type;};
			template<> struct TypeForCode<STRING> {typedef JString type;};
			template<> struct TypeForCode<HASHTABLE> {typedef Hashtable type;};
			template<> struct TypeForCode<DICTIONARY> {typedef DictionaryBase type;};
			template<> struct TypeForCode<OBJECT> {typedef Object* type;};
			template<> struct TypeForCode<ARRAY> {typedef void* type;};
			template<> struct TypeForCode<BYTEARRAY> {typedef nByte* type;};
			template<> struct TypeForCode<CUSTOM> {typedef CustomTypeBase type;};
			template<> struct TypeForCode<UNKNOWN> {typedef Object type;};
		}
		/** @file */
	}
}