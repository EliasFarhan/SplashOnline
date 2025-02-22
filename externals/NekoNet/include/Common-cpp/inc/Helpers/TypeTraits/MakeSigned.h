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
			template<typename T> struct MakeSigned{};
			template<> struct MakeSigned<char>{typedef signed char type;};
			template<> struct MakeSigned<signed char>{typedef signed char type;};
			template<> struct MakeSigned<unsigned char>{typedef signed char type;};
			template<> struct MakeSigned<short>{typedef short type;};
			template<> struct MakeSigned<unsigned short>{typedef short type;};
			template<> struct MakeSigned<int>{typedef int type;};
			template<> struct MakeSigned<unsigned int>{typedef int type;};
			template<> struct MakeSigned<long>{typedef long type;};
			template<> struct MakeSigned<unsigned long>{typedef long type;};
			template<> struct MakeSigned<long long>{typedef long long type;};
			template<> struct MakeSigned<unsigned long long>{typedef long long type;};
#if defined _EG_MICROSOFT_PLATFORM
			template<> struct MakeSigned<wchar_t>{short type;};
#else
			template<> struct MakeSigned<wchar_t>{int type;};
#endif
			template<typename T> struct MakeSigned<T*>{typedef MakeSigned<T>* type;};
			template<typename T> struct MakeSigned<const T>{typedef const MakeSigned<T> type;};
			template<typename T> struct MakeSigned<const T*>{typedef const MakeSigned<T>* type;};

			/** @file */
		}
	}
}
