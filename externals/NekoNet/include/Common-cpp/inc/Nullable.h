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
		namespace Nullable
		{
			template<typename FT>
			const FT& null(void);



			template<typename FT>
			const FT& null(void)
			{
				static FT nullInstance;
				return nullInstance;
			}
		}
	}
}