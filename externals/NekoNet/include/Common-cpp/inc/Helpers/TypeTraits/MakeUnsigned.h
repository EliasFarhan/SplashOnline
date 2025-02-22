/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			template<typename T> struct MakeUnsigned{};
			template<> struct MakeUnsigned<char>{typedef unsigned char type;};
			template<> struct MakeUnsigned<signed char>{typedef unsigned char type;};
			template<> struct MakeUnsigned<unsigned char>{typedef unsigned char type;};
			template<> struct MakeUnsigned<short>{typedef unsigned short type;};
			template<> struct MakeUnsigned<unsigned short>{typedef unsigned short type;};
			template<> struct MakeUnsigned<int>{typedef unsigned int type;};
			template<> struct MakeUnsigned<unsigned int>{typedef unsigned int type;};
			template<> struct MakeUnsigned<long>{typedef unsigned long type;};
			template<> struct MakeUnsigned<unsigned long>{typedef unsigned long type;};
			template<> struct MakeUnsigned<long long>{typedef unsigned long long type;};
			template<> struct MakeUnsigned<unsigned long long>{typedef unsigned long long type;};
#if defined _EG_MICROSOFT_PLATFORM
			template<> struct MakeUnsigned<wchar_t>{unsigned short type;};
#else
			template<> struct MakeUnsigned<wchar_t>{unsigned int type;};
#endif
			template<typename T> struct MakeUnsigned<T*>{typedef MakeUnsigned<T>* type;};
			template<typename T> struct MakeUnsigned<const T>{typedef const MakeUnsigned<T> type;};
			template<typename T> struct MakeUnsigned<const T*>{typedef const MakeUnsigned<T>* type;};

			/** @file */
		}
	}
}
