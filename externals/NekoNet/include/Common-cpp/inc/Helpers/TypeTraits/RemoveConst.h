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
			template<typename T> struct RemoveConst
			{
				typedef T type;
			};

			template<typename T> struct RemoveConst<const T>
			{
				typedef T type;
			};

			template<typename T> struct RemoveConst<const T*>
			{
				typedef T* type;
			};
		}
	}
}