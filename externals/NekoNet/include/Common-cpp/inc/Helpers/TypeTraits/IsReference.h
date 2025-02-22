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
			template<typename Ctype> struct IsReference{static const bool is = false;};
			template<typename Ctype> struct IsReference<Ctype&>{static const bool is = true;};

			/** @file */
		}
	}
}