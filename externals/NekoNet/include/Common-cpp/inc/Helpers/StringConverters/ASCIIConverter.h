/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Base.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class ASCIIConverter : public Base
			{
			public:
				static char* Unicode2ASCII(const EG_CHAR* src, char* dst, unsigned int dstSize);
				static EG_CHAR* ASCII2Unicode(const char* src, EG_CHAR* dst, unsigned int dstLen);
			};
		}
	}
}