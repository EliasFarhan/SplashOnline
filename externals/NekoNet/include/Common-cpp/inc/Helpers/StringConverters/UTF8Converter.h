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
			class UTF8Converter : public Base
			{
			public:
				static char* Unicode2UTF8(const EG_CHAR* src, unsigned int srcLen, char* dst, unsigned int dstSize);
				static EG_CHAR* UTF82Unicode(const char* src, unsigned int srcSize, EG_CHAR* dst, unsigned int dstLen);
				static unsigned int SizeOfWStrAsUTF8(const EG_CHAR* wstr, unsigned int len);
			};
		}
	}
}