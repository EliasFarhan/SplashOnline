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
			template<typename Ctype, Ctype v> struct IntegralConstant{static const Ctype value=v; typedef Ctype valueType; typedef IntegralConstant type; operator valueType() const {return value;}};

			/** @file */
		}
	}
}