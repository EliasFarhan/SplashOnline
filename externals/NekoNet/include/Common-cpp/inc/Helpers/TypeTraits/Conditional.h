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
			template<bool B, typename T, typename F> struct Conditional { typedef T type; };
			template<typename T, typename F> struct Conditional<false, T, F> { typedef F type; };

			/** @file */
		}
	}
}