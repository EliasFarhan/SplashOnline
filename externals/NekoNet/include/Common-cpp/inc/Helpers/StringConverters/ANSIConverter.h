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
			class ANSIConverter : public Base
			{
			public:
				static char* Unicode2ANSI(const EG_CHAR* src, unsigned int srcLen, char* dst, unsigned int dstSize);
				static EG_CHAR* ANSI2Unicode(const char* src, unsigned int srcSize, EG_CHAR* dst, unsigned int dstLen);
#if defined _EG_SONY_PLATFORM || defined EG_DOC
			private:
				static bool mInitialized;
#endif
			};
		}
	}
}